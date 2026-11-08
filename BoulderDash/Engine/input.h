#pragma once

#include "point.h"

#include <algorithm>
#include <vector>

namespace Engine
{
	// Virtual-key codes of the keys the game uses.
	namespace Key
	{
		inline constexpr int Enter = 0x0D;
		inline constexpr int Escape = 0x1B;
		inline constexpr int Space = 0x20;
		inline constexpr int Left = 0x25;
		inline constexpr int Up = 0x26;
		inline constexpr int Right = 0x27;
		inline constexpr int Down = 0x28;
	}

	// The keyboard, the mouse and the window size in the current frame.
	struct Input
	{
		std::vector<int> held;     // keys held down, in the order they were pressed
		std::vector<int> pressed;  // keys pressed since the previous frame
		Point<float> mouse;        // in pixels of the client area, y grows upwards
		bool clicked = false;      // the left button was pressed since the previous frame
		int width = 0, height = 0; // of the client area

		bool wasPressed(int key) const { return std::ranges::contains(pressed, key); }
	};
}
