#include "gameover.h"

#include <list>

#include "menuscreen.h"
#include "..\level.h"
#include "..\..\Engine\texture.h"
#include "..\..\Engine\sprite.h"

namespace BoulderDash
{
	namespace Screens
	{
		Screen* GameOver::update(int timeSpan, std::list<int> kbdEvents, MouseState mouseState)
		{
			for (std::list<int>::const_iterator it = kbdEvents.begin(); it != kbdEvents.end(); ++it)
			{
				if (*it == 13 /* enter */)
					return new MenuScreen(getResources());
			}

			return 0;
		}

		void GameOver::draw(int timeSpan, int width, int height)
		{
			int optimalRectSize = std::min<int>(
				width / Level::minCellsOnScreen,
				height / Level::minCellsOnScreen);

			Engine::Texture* bgTexture = getResources()->getTexture("box");
			for (int j = 0; j < height; j += optimalRectSize)
				for (int i = 0; i < width; i += optimalRectSize)
				{
					Engine::Sprite bgSprite(bgTexture);
					bgSprite.draw(i, j, optimalRectSize, optimalRectSize);
				}

			const float dist = height / 30.f;
			Engine::Texture* titleTexture = getResources()->getTexture("gameOver");
			float titleWidth = width / 2.f;
			float titleHeight = titleWidth / titleTexture->getSourceWidth() * titleTexture->getSourceHeight();
			float titleX = (width - titleWidth) / 2.f;
			float titleY = height - titleHeight - titleX;
			Engine::Sprite titleSprite(titleTexture);
			titleSprite.draw(titleX, titleY, titleWidth, titleHeight);
		}
	}
}
