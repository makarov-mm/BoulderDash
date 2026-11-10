#include "gameover.h"
#include "menuscreen.h"
#include "../ui.h"

namespace BoulderDash::Screens
{
	GameOver::GameOver(const Resources& resources, Reason reason)
		: Screen(resources), m_banner(reason == Reason::TimeIsUp ? resources.timeIsUp : resources.gameOver)
	{ }

	std::unique_ptr<Screen> GameOver::update(int, const Engine::Input& input)
	{
		using namespace Engine::Key;

		if (input.wasPressed(Enter) || input.wasPressed(Escape) || input.wasPressed(Space) || input.clicked)
			return std::make_unique<MenuScreen>(resources());

		return nullptr;
	}

	void GameOver::draw(int width, int height) const
	{
		drawTiles(resources().box, width, height);
		const float bannerWidth = static_cast<float>(width) / 2;
		const float bannerHeight = bannerWidth / m_banner.aspect();
		const float x = (static_cast<float>(width) - bannerWidth) / 2;
		m_banner.draw(x, static_cast<float>(height) - bannerHeight - x, bannerWidth, bannerHeight);
	}
}
