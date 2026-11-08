#include "level.h"
#include "../Engine/platform.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace BoulderDash
{
	namespace
	{
		constexpr int maxSize = 1000; // cells across, so that a broken file cannot ask for gigabytes

		Cell parse(char c)
		{
			switch (c)
			{
			case 'x':
				return {};
			case '1':
				return { Tile::Box };
			case '2':
				return { Tile::Empty, Object::Crystal };
			case '3':
				return { Tile::Empty, Object::Human };
			case '4':
				return { Tile::Empty, Object::Boulder };
			case '5':
				return { Tile::Empty, Object::Door };
			case '6':
				return { Tile::Empty, Object::Ghost };
			case '7':
				return { Tile::Empty, Object::Dark };
			case '8':
				return { Tile::Empty, Object::Heart };
			case '9':
				return { Tile::Empty, Object::Clock };
			default: // '0'
				return { Tile::Ground };
			}
		}
	}

	Level::Level(const std::filesystem::path& file)
	{
		std::ifstream in(file);
		if (!in)
			throw std::runtime_error(std::format("Cannot open {}", Engine::utf8(file)));
		const auto invalid = [&] { return std::runtime_error(std::format("{} is not a level file", Engine::utf8(file))); };

		if (!(in >> m_width >> m_height >> m_timeLimit) || m_width <= 0 || m_height <= 0 || m_width > maxSize || m_height > maxSize || m_timeLimit <= 0)
			throw invalid();

		m_cells.reserve(static_cast<std::size_t>(m_width * m_height));
		for (int y = 0; y < m_height; ++y)
		{
			std::string row;
			if (!(in >> row) || std::ssize(row) < m_width)
				throw invalid();
			for (int x = 0; x < m_width; ++x)
				m_cells.push_back(parse(row[static_cast<std::size_t>(x)]));
		}
	}

	std::filesystem::path Level::file(int id)
	{
		return std::filesystem::path("Levels") / std::format("{}.txt", id);
	}

	int Level::count(Object object) const
	{
		return static_cast<int>(std::ranges::count(m_cells, object, &Cell::object));
	}

	void Level::setCells(std::vector<Cell> cells)
	{
		assert(cells.size() == m_cells.size());
		m_cells = std::move(cells);
	}
}
