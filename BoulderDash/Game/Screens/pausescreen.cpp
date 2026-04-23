#include "pausescreen.h"

#include <list>

#include "..\level.h"

namespace BoulderDash
{
	namespace Screens
	{
		Screen* PauseScreen::update(int timeSpan, std::list<int> kbdEvents, MouseState mouseState)
		{
			if (m_needResume)
				return 0;

			if (m_needSave)
			{
				if (m_parent && m_saveFunc)
					(m_parent->*m_saveFunc)();

				return 0;
			}

			m_mouseState = mouseState;
			return this;
		}

		void PauseScreen::draw(int timeSpan, int width, int height)
		{
			int optimalRectSize = std::min<int>(
				width / Level::minCellsOnScreen,
				height / Level::minCellsOnScreen);

			Engine::Texture* bgTexture = getResources()->getTexture("box");
			for (int j = 0; j < height; j += optimalRectSize)
				for (int i = 0; i < width; i += optimalRectSize)
				{
					Engine::Sprite bgSprite(bgTexture);
					bgSprite.draw(
						static_cast<float>(i), static_cast<float>(j),
						static_cast<float>(optimalRectSize), static_cast<float>(optimalRectSize));
				}

			Engine::Texture* pauseTexture = getResources()->getTexture("pause");
			float pauseWidth = width / 4.f;
			float pauseHeight = pauseWidth / pauseTexture->getSourceWidth() * pauseTexture->getSourceHeight();
			float pauseX = (width - pauseWidth) / 2.f;
			float pauseY = height - pauseHeight * 3;
			Engine::Sprite pauseSprite(pauseTexture);
			pauseSprite.draw(pauseX, pauseY, pauseWidth, pauseHeight);

			float scaleFactor = pauseWidth / pauseTexture->getSourceWidth();
			float resumeSpriteWidth = dynamic_cast<Engine::Texture*>(m_resumeSprite->getTexture())->getSourceWidth() * scaleFactor;
			float resumeSpriteHeight = dynamic_cast<Engine::Texture*>(m_resumeSprite->getTexture())->getSourceHeight() * scaleFactor;
			float offsetY = pauseHeight * 0.5;
			float resumeSpriteX = (width - resumeSpriteWidth) / 2.f;
			float resumeSpriteY = pauseY - offsetY * 4;

			bool resumeSpriteActive =
				m_mouseState.x() >= resumeSpriteX &&
				m_mouseState.x() <= resumeSpriteX + resumeSpriteWidth &&
				height - m_mouseState.y() >= resumeSpriteY &&
				height - m_mouseState.y() <= resumeSpriteY + resumeSpriteHeight;

			if (resumeSpriteActive && m_mouseState.leftButton())
				m_needResume = true;

			(resumeSpriteActive ? m_resumeSpriteActive : m_resumeSprite)->draw(
				resumeSpriteX,
				resumeSpriteY,
				resumeSpriteWidth,
				resumeSpriteHeight);

			float saveSpriteWidth = dynamic_cast<Engine::Texture*>(m_saveSprite->getTexture())->getSourceWidth() * scaleFactor;
			float saveSpriteHeight = dynamic_cast<Engine::Texture*>(m_saveSprite->getTexture())->getSourceHeight() * scaleFactor;
			float saveSpriteX = (width - saveSpriteWidth) / 2.f;
			float saveSpriteY = resumeSpriteY - saveSpriteHeight - offsetY;

			bool saveSpriteActive =
				m_mouseState.x() >= saveSpriteX &&
				m_mouseState.x() <= saveSpriteX + saveSpriteWidth &&
				height - m_mouseState.y() >= saveSpriteY &&
				height - m_mouseState.y() <= saveSpriteY + saveSpriteHeight;

			if (saveSpriteActive && m_mouseState.leftButton())
				m_needSave = true;

			(saveSpriteActive ? m_saveSpriteActive : m_saveSprite)->draw(
				saveSpriteX,
				saveSpriteY,
				saveSpriteWidth,
				saveSpriteHeight);
		}
	}
}