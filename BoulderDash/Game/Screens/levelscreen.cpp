#include "levelscreen.h"
#include "gameover.h"
#include "menuscreen.h"
#include "../../Engine/platform.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>

namespace BoulderDash::Screens
{
	namespace
	{
		// The arrow key held down; the one pressed last if there are several.
		std::optional<Direction> heldDirection(const Engine::Input& input)
		{
			for (const int key : input.held | std::views::reverse)
				switch (key)
				{
				case Engine::Key::Up:
					return Direction::Up;
				case Engine::Key::Right:
					return Direction::Right;
				case Engine::Key::Down:
					return Direction::Down;
				case Engine::Key::Left:
					return Direction::Left;
				}
			return std::nullopt;
		}

		Engine::Point<float> to_float(Engine::Point<int> point)
		{
			return { .x = static_cast<float>(point.x), .y = static_cast<float>(point.y) };
		}

		Engine::Point<float> offset_of(const std::optional<Moving>& moving)
		{
			return moving ? moving->offset() : Engine::Point<float>{};
		}

		// Where a level `size` pixels long starts on a screen `screen` pixels long, to have the point `target` of the
		// level in the middle, but no gaps at the edges. A level smaller than the screen is centred.
		float scroll(float screen, float size, float target)
		{
			if (size <= screen)
				return (screen - size) / 2;

			return std::clamp(screen / 2 - target, screen - size, 0.f);
		}
	}

	LevelScreen::LevelScreen(const Resources& resources, int id, int lives)
		: Screen(resources),
		  m_id(id),
		  m_level(Level::file(id)),
		  m_lives(lives),
		  m_timeLeft(m_level.timeLimit() * 1000),
		  m_totalCrystals(m_level.count(Object::Crystal)),
		  m_doorOpen(m_totalCrystals == 0)
	{
		if (m_level.count(Object::Human) != 1)
			throw std::runtime_error(std::format("{} must have one human", Engine::utf8(Level::file(id))));

		find_actors();
		m_start = m_human;
	}

	std::unique_ptr<LevelScreen> LevelScreen::load(const Resources& resources)
	{
		std::ifstream in(saveFile);
		int id = 0, lives = 0, time = 0, crystals = 0, totalCrystals = 0;

		if (in >> id >> lives >> time >> crystals >> totalCrystals && id > 0 && lives > 0 && lives <= maxLives && time > 0 && crystals >= 0 && std::filesystem::exists(Level::file(id)))
		{
			auto screen = std::make_unique<LevelScreen>(resources, id, lives);

			if (screen->restore(in, time, crystals))
				return screen;
		}

		return std::make_unique<LevelScreen>(resources, 1);
	}

	bool LevelScreen::restore(std::istream& in, int time, int crystals)
	{
		std::vector<Cell> cells;
		cells.reserve(m_level.cells().size());

		while (cells.size() < m_level.cells().size())
		{
			int code = 0;

			if (!(in >> code))
				return false;

			const std::optional<Cell> cell = decode(code);

			if (!cell)
				return false;

			cells.push_back(*cell);
		}

		if (std::ranges::count(cells, Object::Human, &Cell::object) != 1)
			return false;

		m_level.setCells(std::move(cells));
		m_timeLeft = time;
		m_crystals = crystals;
		m_doorOpen = m_level.count(Object::Crystal) == 0;
		find_actors();
		return true;
	}

	void LevelScreen::save() const
	{
		std::ofstream out(saveFile);
		out << std::format("{} {} {} {} {}\n", m_id, m_lives, m_timeLeft, m_crystals, m_totalCrystals);

		for (int y = 0; y < m_level.height(); ++y)
		{
			for (int x = 0; x < m_level.width(); ++x)
				out << encode(m_level.at({ .x = x, .y = y })) << ' ';

			out << '\n';
		}

		out.close();
		if (!out)
			Engine::showError(std::format("Cannot save the game to {}", saveFile));
	}

	void LevelScreen::find_actors()
	{
		m_enemies.clear();

		for (int y = 0; y < m_level.height(); ++y)
			for (int x = 0; x < m_level.width(); ++x)
			{
				const Object object = m_level.at({ .x = x, .y = y }).object;

				if (object == Object::Human)
					m_human = { .x = x, .y = y };

				else if (isEnemy(object))
					m_enemies.emplace_back(object, Engine::Point{ .x = x, .y = y });
			}
	}

	std::unique_ptr<Screen> LevelScreen::update(int time, const Engine::Input& input)
	{
		if (m_pause)
		{
			switch (m_pause->update(input))
			{
			case PauseScreen::Choice::Save:
				save();
				m_pause.reset();
				break;

			case PauseScreen::Choice::Resume:
				m_pause.reset();
				break;

			case PauseScreen::Choice::None:
				break;
			}

			return nullptr;
		}

		if (input.wasPressed(Engine::Key::Escape))
		{
			m_pause.emplace(resources());
			return nullptr;
		}

		if ((m_timeLeft -= time) <= 0)
			return std::make_unique<GameOver>(resources(), GameOver::Reason::TimeIsUp);

		m_clock += time;

		if (m_humanMoving)
		{
			m_humanMoving->update(time);

			if (m_humanMoving->done())
				m_humanMoving.reset();
		}

		if (!m_humanMoving)
			if (const std::optional<Direction> direction = heldDirection(input))
				if (auto next = moveHuman(*direction))
					return next;

		if (std::unique_ptr<Screen> next = moveEnemies(time))
			return next;

		if (std::unique_ptr<Screen> next = dropObjects(time))
			return next;

		m_doorOpen = m_level.count(Object::Crystal) == 0;

		if (m_doorOpen)
			m_doorClock += time;

		if (m_humanMoving)
			m_humanWalkTime += time;

		return nullptr;
	}

	bool LevelScreen::humanCanEnter(Cell cell) const
	{
		if (cell.tile == Tile::Box)
			return false;

		switch (cell.object)
		{
		case Object::Boulder:
			return false;
		case Object::Door:
			return m_doorOpen; // the closed door is a part of the wall
		default:
			return true;
		}
	}

	std::unique_ptr<Screen> LevelScreen::moveHuman(Direction direction)
	{
		const Engine::Point<int> from = m_human;
		const Engine::Point<int> to = from + step(direction);
		const Cell target = m_level.at(to);
		if (!humanCanEnter(target))
			return nullptr;

		switch (target.object)
		{
		case Object::Door:
			return nextLevel();
		case Object::Ghost:
		case Object::Dark:
			m_level.setObject(from, Object::None);
			return loseLife();
		case Object::Crystal:
			++m_crystals;
			break;
		case Object::Heart:
			m_lives = std::min(m_lives + 1, maxLives);
			break;
		case Object::Clock:
			m_timeLeft += clockBonus;
			break;
		default:
			break;
		}

		m_level.set(from, {});
		m_level.set(to, { Tile::Empty, Object::Human });
		m_human = to;
		m_humanMoving.emplace(from, to, humanStepTime);
		m_humanDirection = direction;
		return nullptr;
	}

	std::unique_ptr<Screen> LevelScreen::moveEnemies(int time)
	{
		for (Enemy& enemy : m_enemies)
		{
			const std::optional<Engine::Point<int>> target = enemy.update(time, m_level, m_human, m_random);
			if (!target || !Enemy::canEnter(m_level.at(*target)))
				continue;

			const bool catches = *target == m_human;
			m_level.setObject(enemy.position(), Object::None);
			m_level.setObject(*target, enemy.kind());
			enemy.move_to(*target);
			if (catches)
				if (auto next = loseLife())
					return next;
		}
		return nullptr;
	}

	std::unique_ptr<Screen> LevelScreen::dropObjects(int time)
	{
		for (Moving& falling : m_falling)
			falling.update(time);

		for (int x = 0; x < m_level.width(); ++x)
			for (int y = m_level.height() - 2; y >= 0; --y)
			{
				const Engine::Point<int> cell{ x, y };
				const Engine::Point<int> below{ x, y + 1 };
				const Object object = m_level.at(cell).object;
				if (object != Object::Boulder && object != Object::Crystal)
					continue;

				// An object still falling into its cell waits; a boulder that has just landed on the human crushes them.
				const auto falling = std::ranges::find(m_falling, cell, &Moving::to);
				const bool fell = falling != m_falling.end();
				if (fell && !falling->done())
					continue;
				const bool crushes = object == Object::Boulder && fell && below == m_human;
				if (!crushes && !m_level.at(below).isEmpty())
					continue;

				m_level.setObject(cell, Object::None);
				m_level.setObject(below, object);
				m_falling.emplace_back(cell, below, fallTime);
				if (crushes)
					if (auto next = loseLife())
						return next;
			}

		std::erase_if(m_falling, [](const Moving& moving) { return moving.done(); });
		return nullptr;
	}

	// The human has been removed from the level already.
	std::unique_ptr<Screen> LevelScreen::loseLife()
	{
		if (--m_lives <= 0)
			return std::make_unique<GameOver>(resources(), GameOver::Reason::NoLives);

		m_humanMoving.reset();
		m_human = freeCellNear(m_start);
		m_level.set(m_human, { Tile::Empty, Object::Human });
		return nullptr;
	}

	std::unique_ptr<Screen> LevelScreen::nextLevel() const
	{
		if (!std::filesystem::exists(Level::file(m_id + 1)))
			return std::make_unique<MenuScreen>(resources()); // the last level is done
		return std::make_unique<LevelScreen>(resources(), m_id + 1, m_lives);
	}

	Engine::Point<int> LevelScreen::freeCellNear(Engine::Point<int> start) const
	{
		// Breadth-first search through everything but walls
		std::vector<bool> seen(m_level.cells().size());
		const auto see = [&](Engine::Point<int> cell)
		{
			const auto index = static_cast<std::size_t>(cell.y * m_level.width() + cell.x);
			if (seen[index])
				return false;
			seen[index] = true;
			return true;
		};

		std::vector<Engine::Point<int>> queue{ start };
		see(start);
		for (std::size_t i = 0; i < queue.size(); ++i)
		{
			if (m_level.at(queue[i]).object == Object::None)
				return queue[i];
			for (Direction direction : directions)
			{
				const Engine::Point<int> next = queue[i] + step(direction);
				if (m_level.at(next).tile != Tile::Box && see(next))
					queue.push_back(next);
			}
		}
		return start;
	}

	void LevelScreen::draw(int width, int height) const
	{
		if (m_pause)
		{
			m_pause->draw(width, height);
			return;
		}

		const int size = cellSize(width, height);
		if (size <= 0)
			return;

		// The view follows the human. `left` and `top` are where the level's top left corner is on the screen.
		const float cell = static_cast<float>(size);
		const float screenWidth = static_cast<float>(width);
		const float screenHeight = static_cast<float>(height);
		const Engine::Point<float> human = to_float(m_human) + offset_of(m_humanMoving);
		const float left = scroll(screenWidth, static_cast<float>(m_level.width()) * cell, (human.x + 0.5f) * cell);
		const float top = screenHeight - scroll(screenHeight, static_cast<float>(m_level.height()) * cell, (human.y + 0.5f) * cell);
		const auto rect = [&](Engine::Point<int> position, Engine::Point<float> offset)
		{
			const Engine::Point<float> at = to_float(position) + offset;
			return Engine::Rect{ left + at.x * cell, top - (at.y + 1) * cell, cell, cell };
		};

		// The cells on the screen, and one more on each side for the objects moving in
		const int firstX = std::max(static_cast<int>(std::floor(-left / cell)) - 1, 0);
		const int lastX = std::min(static_cast<int>(std::floor((screenWidth - left) / cell)) + 1, m_level.width() - 1);
		const int firstY = std::max(static_cast<int>(std::floor((top - screenHeight) / cell)) - 1, 0);
		const int lastY = std::min(static_cast<int>(std::floor(top / cell)) + 1, m_level.height() - 1);

		const Resources& images = resources();
		for (int y = firstY; y <= lastY; ++y)
			for (int x = firstX; x <= lastX; ++x)
			{
				const Tile tile = m_level.at({ x, y }).tile;
				const Engine::Texture& texture = tile == Tile::Ground ? images.ground : tile == Tile::Box ? images.box : images.background;
				texture.draw(rect({ x, y }, {}));
			}

		for (int y = firstY; y <= lastY; ++y)
			for (int x = firstX; x <= lastX; ++x)
				if (const Engine::Texture* texture = objectTexture(m_level.at({ x, y }).object))
				{
					const auto falling = std::ranges::find(m_falling, Engine::Point<int>{ x, y }, &Moving::to);
					texture->draw(rect({ x, y }, falling != m_falling.end() ? falling->offset() : Engine::Point<float>{}));
				}

		for (const Enemy& enemy : m_enemies)
		{
			const auto& walking = enemy.kind() == Object::Ghost ? images.ghost : images.dark;
			walking[static_cast<std::size_t>(enemy.direction())].frame(enemy.walkTime()).draw(rect(enemy.position(), offset_of(enemy.moving())));
		}

		images.human[static_cast<std::size_t>(m_humanDirection)].frame(m_humanWalkTime).draw(rect(m_human, offset_of(m_humanMoving)));

		drawPanel(width, height);
	}

	// The image of an object lying in a cell; the human and the enemies are drawn separately, over everything.
	const Engine::Texture* LevelScreen::objectTexture(Object object) const
	{
		const Resources& images = resources();
		switch (object)
		{
		case Object::Crystal:
			return &images.crystalAnimation.frame(m_clock);
		case Object::Boulder:
			return &images.boulder;
		case Object::Heart:
			return &images.heartAnimation.frame(m_clock);
		case Object::Clock:
			return &images.clock;
		case Object::Door:
			return m_doorOpen ? &images.doorAnimation.frame(m_doorClock) : &images.box; // the closed door looks like a wall
		default:
			return nullptr;
		}
	}

	// Lives, crystals and time, over the top of the screen.
	void LevelScreen::drawPanel(int width, int height) const
	{
		const Resources& images = resources();
		const float screenHeight = static_cast<float>(height);
		const float panel = std::floor(screenHeight / 20);
		const float panelY = screenHeight - panel;
		images.panel.draw(0, panelY, static_cast<float>(width), panel);

		float x = 0;
		for (int i = 0; i < maxLives; ++i)
		{
			(i < m_lives ? images.heart : images.emptyHeart).draw(x, panelY, panel, panel);
			x += panel * 1.1f;
		}

		const float digitHeight = panel / 2;
		const float digitY = panelY + (panel - digitHeight) / 2;

		x += panel * 2;
		images.crystal.draw(x, panelY, panel, panel);
		x = drawNumber(m_crystals, x + panel * 1.3f, digitY, digitHeight);
		const float slashWidth = images.slash.aspect() * digitHeight;
		images.slash.draw(x + slashWidth * 0.1f, digitY, slashWidth, digitHeight);
		x = drawNumber(m_totalCrystals, x + slashWidth * 1.2f, digitY, digitHeight);

		x += panel * 5 / 3;
		images.clock.draw(x, panelY, images.clock.aspect() * panel, panel);
		drawNumber(m_timeLeft / 1000, x + panel * 5 / 3, digitY, digitHeight);
	}

	// Draws a number from x on; returns where it ends.
	float LevelScreen::drawNumber(int number, float x, float y, float height) const
	{
		for (const char digit : std::to_string(std::max(number, 0)))
		{
			const Engine::Texture& image = resources().digits[static_cast<std::size_t>(digit - '0')];
			const float width = image.aspect() * height;
			image.draw(x, y, width, height);
			x += width * 1.3f;
		}
		return x;
	}
}
