#ifndef PAUSESCREEN_H
#define PAUSESCREEN_H

#include <list>

#include "..\screen.h"
#include "..\..\Engine\sprite.h"
#include "..\resources.h"

namespace BoulderDash
{
	namespace Screens
	{
		class PauseScreen : public Screen
		{
		private:
			Engine::Sprite *m_pauseSprite;
			Engine::Sprite *m_resumeSprite, *m_resumeSpriteActive, *m_saveSprite, *m_saveSpriteActive;

			MouseState m_mouseState;

			bool m_needResume;
			bool m_needSave;

			void (Screen::*m_saveFunc)();
			Screen* m_parent;
		public:
			PauseScreen(Resources *resources, Screen* parent, void (Screen::*saveFunc)())
				: Screen(resources)
			{
				m_parent = parent;
				m_saveFunc = saveFunc;
				m_pauseSprite = new Engine::Sprite(resources->getTexture("pause"));
				m_resumeSprite = new Engine::Sprite(resources->getTexture("buttons", "resume"));
				m_resumeSpriteActive = new Engine::Sprite(resources->getTexture("buttons", "resumeActive"));
				m_saveSprite = new Engine::Sprite(resources->getTexture("buttons", "save"));
				m_saveSpriteActive = new Engine::Sprite(resources->getTexture("buttons", "saveActive"));

				m_needResume = m_needSave = false;
			}

			~PauseScreen() override
			{
				delete m_pauseSprite;

				delete m_resumeSprite;
				delete m_resumeSpriteActive;

				delete m_saveSprite;
				delete m_saveSpriteActive;
			}

			virtual Screen* update(int timeSpan, std::list<int> kbdEvents, MouseState mouseState);
			virtual void draw(int timeSpan, int width, int height);
		};
	}
}

#endif