#include "animation.h"

namespace Engine
{
	Animation::~Animation()
	{
		for (auto& m_texture : m_textures)
			delete m_texture;
	}

	void Animation::update(int timeSpan)
	{
		m_timeSpan += timeSpan;

		while (m_timeSpan > m_frameTime)
		{
			m_timeSpan -= m_frameTime;
			++m_frame;

			if (m_frame >= static_cast<int>(m_textures.size()))
				m_frame = 0;
		}
	}

	void Animation::reset()
	{
		m_timeSpan = 0;
		m_frame = 0;
	}
}
