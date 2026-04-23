#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <list>

#include "..\screen.h"
#include "..\resources.h"
#include "..\..\Engine\sprite.h"
#include "..\mouseState.h"

namespace BoulderDash
{
	namespace Screens
	{
		class MenuScreen: public Screen
		{
		public:
			Engine::Sprite *m_newGameSprite, *m_newGameSpriteActive, *m_loadSprite, *m_loadSpriteActive;

			MouseState m_mouseState;

			bool m_needLoad;
			bool m_needNew;
		public:
			MenuScreen(Resources *resources)
				: Screen(resources)
			{
				m_newGameSprite = new Engine::Sprite(resources->getTexture("buttons", "newGame"));
				m_newGameSpriteActive = new Engine::Sprite(resources->getTexture("buttons", "newGameActive"));

				m_loadSprite = new Engine::Sprite(resources->getTexture("buttons", "load"));
				m_loadSpriteActive = new Engine::Sprite(resources->getTexture("buttons", "loadActive"));

				m_needLoad = m_needNew = false;
			}

			~MenuScreen()
			{
				delete m_newGameSprite;
				delete m_newGameSpriteActive;

				delete m_loadSprite;
				delete m_loadSpriteActive;
			}

			virtual Screen* update(int timeSpan, std::list<int> kbdEvents, MouseState mouseState);
			virtual void draw(int timeSpan, int width, int height);
		};
	}
}

#endif