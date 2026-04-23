#ifndef TIMEISUP_H
#define TIMEISUP_H

#include <list>

#include "..\screen.h"
#include "..\resources.h"

namespace BoulderDash
{
	namespace Screens
	{
		class TimeIsUp: public Screen
		{
		public:
			TimeIsUp(Resources *resources) : Screen(resources) { }
			virtual Screen* update(int timeSpan, std::list<int> kbdEvents, MouseState mouseState);
			virtual void draw(int timeSpan, int width, int height);
		};
	}
}

#endif