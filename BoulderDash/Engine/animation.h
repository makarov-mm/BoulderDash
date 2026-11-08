#pragma once

#include "texture.h"

#include <cstddef>
#include <filesystem>
#include <vector>

namespace Engine
{
	// A looping sequence of images.
	class Animation
	{
	public:
		// Loads directory/frame0.png ... frame<count - 1>.png.
		Animation(const std::filesystem::path& directory, int count, int framesPerSecond);

		// The frame shown `time` milliseconds after the start.
		const Texture& frame(int time) const
		{
			return m_frames[static_cast<std::size_t>(time / m_frameTime) % m_frames.size()];
		}

		const Texture& operator[](std::size_t index) const { return m_frames[index]; }

	private:
		std::vector<Texture> m_frames;
		int m_frameTime; // milliseconds
	};
}
