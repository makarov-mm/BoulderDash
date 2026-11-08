#pragma once

#include <algorithm>

namespace BoulderDash
{
	inline constexpr int cellsOnScreen = 12; // a cell is 1/12 of the smaller side of the window

	inline constexpr int startLives = 2;
	inline constexpr int maxLives = 3;

	// Times in milliseconds
	inline constexpr int humanStepTime = 200; // to move to the next cell
	inline constexpr int fallTime = 300;      // for a boulder or a crystal to fall by a cell
	inline constexpr int enemyStepTime = 1000;
	inline constexpr int clockBonus = 10'000;

	inline constexpr int animationFps = 15;

	inline constexpr const char* saveFile = "save.txt";

	// The size of a cell on the screen, in pixels.
	constexpr int cellSize(int width, int height)
	{
		return std::min(width, height) / cellsOnScreen;
	}
}
