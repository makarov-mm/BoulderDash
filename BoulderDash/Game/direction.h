#pragma once

#include "../Engine/point.h"

#include <utility>

namespace BoulderDash
{
	// The values number the sprite folders: Gfx/human/0 faces up, 1 right, 2 down, 3 left.
	enum class Direction
	{
		Up = 0,
		Right = 1,
		Down = 2,
		Left = 3,
	};

	inline constexpr Direction directions[] = { Direction::Up, Direction::Right, Direction::Down, Direction::Left };

	// The step to the next cell; y grows downwards in a level.
	constexpr Engine::Point<int> step(Direction direction)
	{
		switch (direction)
		{
		case Direction::Up:
			return { .x = 0, .y = -1 };
		case Direction::Right:
			return { .x = 1, .y = 0 };
		case Direction::Down:
			return { .x = 0, .y = 1 };
		case Direction::Left:
			return { .x = -1, .y = 0 };
		}

		std::unreachable();
	}
}
