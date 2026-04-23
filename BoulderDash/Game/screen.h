#ifndef SCREEN_H
#define SCREEN_H

#include "resources.h"
#include "mouseState.h"

#include <list>

namespace BoulderDash
{
	class Screen
	{
	private:
		Resources *m_resources;
	protected:
		Resources *getResources() const { return m_resources; }
	public:
		Screen(Resources *resources)
			: m_resources(resources)
		{ }

		virtual ~Screen() = default;
		virtual Screen* update(int timeSpan, std::list<int> kbdEvents, MouseState mouseState) = 0;
		virtual void draw(int timeSpan, int width, int height) = 0;
	};
}

#endif