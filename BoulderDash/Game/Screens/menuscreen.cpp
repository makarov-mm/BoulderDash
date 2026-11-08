#include "menuscreen.h"
#include "levelscreen.h"

namespace BoulderDash::Screens
{
	namespace
	{
		constexpr std::size_t newGameButton = 0, loadButton = 1;
	}

	MenuScreen::MenuScreen(const Resources& resources)
		: Screen(resources),
		  m_menu(resources.box, resources.title, 1 / 2.f, {
			  { resources.newGame, resources.newGameHovered },
			  { resources.load, resources.loadHovered },
		  })
	{
	}

	std::unique_ptr<Screen> MenuScreen::update(int, const Engine::Input& input)
	{
		const std::optional<std::size_t> clicked = m_menu.update(input);
		if (clicked == newGameButton)
			return std::make_unique<LevelScreen>(resources(), 1);
		if (clicked == loadButton)
			return LevelScreen::load(resources());
		return nullptr;
	}

	void MenuScreen::draw(int width, int height) const
	{
		m_menu.draw(width, height);
	}
}
