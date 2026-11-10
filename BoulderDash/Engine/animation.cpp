#include "animation.h"

#include <format>

namespace Engine
{
	Animation::Animation(const std::filesystem::path& directory, int count, int framesPerSecond)
		: m_frameTime(1000 / framesPerSecond)
	{
		m_frames.reserve(static_cast<std::size_t>(count));

		for (int i = 0; i < count; ++i)
			m_frames.emplace_back(directory / std::format("frame{}.png", i));
	}
}
