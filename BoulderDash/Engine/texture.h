#pragma once

#include "point.h"

#include <filesystem>

namespace Engine
{
	// An image in an OpenGL texture.
	class Texture
	{
	public:
		// Loads a PNG (or any image Windows can decode); throws if it cannot.
		explicit Texture(const std::filesystem::path& file);
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;
		~Texture();

		int width() const { return m_width; }
		int height() const { return m_height; }
		float aspect() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

		// Draws the image stretched over a rectangle; y grows upwards.
		void draw(float x, float y, float width, float height) const;
		void draw(const Rect& rect) const { draw(rect.x, rect.y, rect.width, rect.height); }

	private:
		unsigned int m_id = 0;
		int m_width = 0, m_height = 0;
	};
}
