#include "texture.h"
#include "platform.h"

#include <windows.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <GL/gl.h>

#include <format>
#include <stdexcept>
#include <utility>
#include <vector>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#endif

namespace Engine
{
	namespace
	{
		unsigned int s_bound = 0; // the texture bound last, so as not to bind it again

		void bind(unsigned int id)
		{
			if (id != s_bound)
			{
				glBindTexture(GL_TEXTURE_2D, id);
				s_bound = id;
			}
		}

		// Decodes an image into RGBA with premultiplied alpha, top row first.
		std::vector<BYTE> decode(const std::filesystem::path& file, UINT& width, UINT& height)
		{
			using Microsoft::WRL::ComPtr;
			const auto check = [&](HRESULT result)
			{
				if (FAILED(result))
					throw std::runtime_error(std::format("Cannot load {}", utf8(file)));
			};

			ComPtr<IWICImagingFactory> factory;
			check(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));
			ComPtr<IWICBitmapDecoder> decoder;
			check(factory->CreateDecoderFromFilename(file.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder));
			ComPtr<IWICBitmapFrameDecode> frame;
			check(decoder->GetFrame(0, &frame));
			ComPtr<IWICFormatConverter> converter;
			check(factory->CreateFormatConverter(&converter));
			check(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom));
			check(converter->GetSize(&width, &height));

			std::vector<BYTE> pixels(std::size_t{ width } * height * 4);
			check(converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.size()), pixels.data()));
			return pixels;
		}
	}

	Texture::Texture(const std::filesystem::path& file)
	{
		UINT width = 0, height = 0;
		const std::vector<BYTE> pixels = decode(file, width, height);
		m_width = static_cast<int>(width);
		m_height = static_cast<int>(height);

		glGenTextures(1, &m_id);
		bind(m_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	}

	Texture::Texture(Texture&& other) noexcept
		: m_id(std::exchange(other.m_id, 0)), m_width(other.m_width), m_height(other.m_height)
	{
	}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		std::swap(m_id, other.m_id);
		std::swap(m_width, other.m_width);
		std::swap(m_height, other.m_height);
		return *this;
	}

	Texture::~Texture()
	{
		if (m_id == 0)
			return;
		if (s_bound == m_id)
			s_bound = 0;
		glDeleteTextures(1, &m_id);
	}

	void Texture::draw(float x, float y, float width, float height) const
	{
		bind(m_id);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1);
		glVertex2f(x, y);
		glTexCoord2f(1, 1);
		glVertex2f(x + width, y);
		glTexCoord2f(1, 0);
		glVertex2f(x + width, y + height);
		glTexCoord2f(0, 0);
		glVertex2f(x, y + height);
		glEnd();
	}
}
