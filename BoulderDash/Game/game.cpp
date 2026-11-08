#include "game.h"
#include "Screens/menuscreen.h"

namespace BoulderDash
{
	Game::Game()
		: GameBase(L"BoulderDash", 800, 600),
		  m_screen(std::make_unique<Screens::MenuScreen>(m_resources))
	{
	}

	void Game::update(int time, const Engine::Input& input)
	{
		if (auto next = m_screen->update(time, input))
			m_screen = std::move(next);
	}

	void Game::draw(int width, int height)
	{
		m_screen->draw(width, height);
	}
}
