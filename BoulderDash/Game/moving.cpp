#include "moving.h"

#include <algorithm>

namespace BoulderDash
{
	Engine::Point<float> Moving::offset() const
	{
		const float left = std::clamp(static_cast<float>(m_left) / static_cast<float>(m_duration), 0.f, 1.f);
		const Engine::Point<int> back = m_from - m_to;

		return {
			.x = static_cast<float>(back.x) * left,
			.y = static_cast<float>(back.y) * left
		};
	}
}
