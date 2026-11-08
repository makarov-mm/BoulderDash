#include "enemy.h"

#include <iterator>
#include <vector>

namespace BoulderDash
{
	std::optional<Engine::Point<int>> Enemy::update(int time, const Level& level, Engine::Point<int> human, std::mt19937& random)
	{
		if (m_moving)
		{
			m_moving->update(time);
			m_walkTime += time;
			if (m_moving->done())
				m_moving.reset();
		}

		if ((m_stepTimer -= time) > 0)
			return std::nullopt;
		m_stepTimer = enemyStepTime;
		return m_position + step(directionTo(level, human, random));
	}

	void Enemy::moveTo(Engine::Point<int> cell)
	{
		const Engine::Point<int> shift = cell - m_position;
		m_direction = shift.x > 0 ? Direction::Right
			: shift.x < 0 ? Direction::Left
			: shift.y < 0 ? Direction::Up
			: Direction::Down;
		m_moving.emplace(m_position, cell, enemyStepTime);
		m_position = cell;
	}

	Direction Enemy::directionTo(const Level& level, Engine::Point<int> human, std::mt19937& random) const
	{
		// Breadth-first search from the enemy through the cells it can enter, until it reaches the human
		std::vector<int> distances(static_cast<std::size_t>(level.width() * level.height()), -1);
		const auto distance = [&](Engine::Point<int> cell) -> int&
		{
			return distances[static_cast<std::size_t>(cell.y * level.width() + cell.x)];
		};

		std::vector<Engine::Point<int>> queue{ m_position };
		distance(m_position) = 0;
		bool found = false;
		for (std::size_t i = 0; i < queue.size() && !found; ++i)
			for (Direction direction : directions)
			{
				const Engine::Point<int> next = queue[i] + step(direction);
				if (!canEnter(level.at(next)) || distance(next) >= 0)
					continue;
				distance(next) = distance(queue[i]) + 1;
				if (next == human)
				{
					found = true;
					break;
				}
				queue.push_back(next);
			}

		if (!found)
			return directions[std::uniform_int_distribution<std::size_t>(0, std::size(directions) - 1)(random)];

		// Walk the path back from the human to the cell next to the enemy
		Engine::Point<int> cell = human;
		while (distance(cell) > 1)
			for (Direction direction : directions)
			{
				const Engine::Point<int> previous = cell + step(direction);
				if (level.contains(previous) && distance(previous) == distance(cell) - 1)
				{
					cell = previous;
					break;
				}
			}

		for (Direction direction : directions)
			if (m_position + step(direction) == cell)
				return direction;
		return Direction::Down;
	}
}
