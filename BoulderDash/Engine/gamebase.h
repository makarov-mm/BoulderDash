#pragma once

#include "input.h"

#include <memory>

namespace Engine
{
	// A window with an OpenGL context and the game loop: every frame the game is updated, then drawn.
	class GameBase
	{
	public:
		// Opens a window with a client area of width x height, scaled for the screen DPI.
		GameBase(const wchar_t* title, int width, int height);
		virtual ~GameBase();

		GameBase(const GameBase&) = delete;
		GameBase& operator=(const GameBase&) = delete;

		// Runs the game until the window is closed.
		void run();

	protected:
		// Advances the game by `time` milliseconds.
		virtual void update(int time, const Input& input) = 0;
		// Draws the game; (0, 0) is the bottom left corner of the client area, (width, height) the top right one.
		virtual void draw(int width, int height) = 0;

	private:
		struct Window;
		std::unique_ptr<Window> m_window_;
	};
}
