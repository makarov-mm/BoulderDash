#pragma once

#include "cellType.h"
#include "../Engine/point.h"

#include <cassert>
#include <cstddef>
#include <filesystem>
#include <vector>

namespace BoulderDash
{
	// A grid of cells; x grows to the right, y downwards.
	class Level
	{
	public:
		// Reads a level file: "<width> <height>", the time limit in seconds, then a line of cell characters per row.
		explicit Level(const std::filesystem::path& file);

		// Levels/<id>.txt
		static std::filesystem::path file(int id);

		int width() const { return m_width; }
		int height() const { return m_height; }
		int timeLimit() const { return m_timeLimit; } // seconds

		bool contains(Engine::Point<int> cell) const
		{
			return cell.x >= 0 && cell.y >= 0 && cell.x < m_width && cell.y < m_height;
		}

		// Outside the level there are walls.
		Cell at(Engine::Point<int> cell) const { return contains(cell) ? m_cells[index(cell)] : Cell{ Tile::Box }; }

		void set(Engine::Point<int> cell, Cell value) { m_cells[index(cell)] = value; }
		void setObject(Engine::Point<int> cell, Object object) { m_cells[index(cell)].object = object; }

		int count(Object object) const;

		// Row by row
		const std::vector<Cell>& cells() const { return m_cells; }
		void setCells(std::vector<Cell> cells);

	private:
		std::size_t index(Engine::Point<int> cell) const
		{
			assert(contains(cell));
			return static_cast<std::size_t>(cell.y * m_width + cell.x);
		}

		int m_width = 0, m_height = 0;
		int m_timeLimit = 0;
		std::vector<Cell> m_cells;
	};
}
