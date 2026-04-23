#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <list>

#include "..\screen.h"
#include "..\resources.h"

namespace BoulderDash
{
	namespace Screens
	{
		class GameOver final: public Screen
		{
		public:
			GameOver(Resources *resources) : Screen(resources) { }
			~GameOver() override = default;
			virtual Screen* update(int timeSpan, std::list<int> kbdEvents, MouseState mouseState);
			virtual void draw(int timeSpan, int width, int height);
		};
	}
}

#endif