#include "Engine\gameBase.h"
#include "Game\game.h"
#include <memory>

int main()
{
	HWND console = GetConsoleWindow();

	if (console != nullptr)
		ShowWindow(console, SW_HIDE);

	std::unique_ptr<Engine::GameBase> game(new BoulderDash::Game());
	game->run();
}