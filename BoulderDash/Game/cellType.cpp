#include "cellType.h"

#include <utility>

namespace BoulderDash
{
	int encode(Cell cell)
	{
		return std::to_underlying(cell.object) << 8 | std::to_underlying(cell.tile);
	}

	std::optional<Cell> decode(int code)
	{
		const int tile = code & 0xFF;
		const int object = code >> 8;

		if (code < 0 || std::cmp_greater(tile, std::to_underlying(Tile::Box)) || std::cmp_greater(object, std::to_underlying(Object::Clock)))
			return std::nullopt;

		return Cell { .tile = static_cast<Tile>(tile), .object = static_cast<Object>(object) };
	}
}
