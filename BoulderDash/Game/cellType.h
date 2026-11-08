#pragma once

#include <cstdint>
#include <optional>

namespace BoulderDash
{
	// What a cell is made of.
	enum class Tile : std::uint8_t
	{
		Empty = 0,
		Ground = 1, // the human digs through it
		Box = 2,    // a wall
	};

	// What stands in a cell.
	enum class Object : std::uint8_t
	{
		None = 0,
		Human = 1,
		Crystal = 2,
		Boulder = 3,
		Door = 4,
		Ghost = 5,
		Dark = 6,
		Heart = 7,
		Clock = 8,
	};

	struct Cell
	{
		Tile tile = Tile::Empty;
		Object object = Object::None;

		bool operator==(const Cell&) const = default;
		bool isEmpty() const { return *this == Cell{}; }
	};

	constexpr bool isEnemy(Object object)
	{
		return object == Object::Ghost || object == Object::Dark;
	}

	// The cell codes of save files: the tile in the low byte, the object in the next one.
	int encode(Cell cell);
	std::optional<Cell> decode(int code);
}
