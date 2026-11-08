#pragma once

#include "../Engine/point.h"

namespace BoulderDash
{
	// The animation of an object moving to the next cell. The level has the object in the new cell already.
	class Moving
	{
	public:
		Moving(Engine::Point<int> from, Engine::Point<int> to, int duration)
			: m_from(from), m_to(to), m_duration(duration), m_left(duration)
		{ }

		void update(int time) { m_left -= time; }
		bool done() const { return m_left <= 0; }

		Engine::Point<int> from() const { return m_from; }
		Engine::Point<int> to() const { return m_to; }

		// Where the object is drawn, relative to the new cell, in cells.
		Engine::Point<float> offset() const;

	private:
		Engine::Point<int> m_from, m_to;
		int m_duration;
		int m_left; // ms
	};
}
