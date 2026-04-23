#include <cstdio>
#include <algorithm>
#include <windows.h>
#include <gl/gl.h>

#include "texture.h"
#include "..\FreeImage\FreeImage.h"

#define GL_CLAMP_TO_EDGE 0x812f

namespace Engine
{
	int Texture::m_texturesCounter = 0;
	Texture* Texture::m_active = 0;

	Texture::Texture()
	{
		glGenTextures(1, &n);
		glBindTexture(GL_TEXTURE_2D, n);

		#ifdef FREEIMAGE_LIB
		if (!m_texturesCounter)
			FreeImage_Initialise();
		++m_texturesCounter;
		#endif
	}

	Texture::~Texture()
	{
		#ifdef FREEIMAGE_LIB
		if (!--m_texturesCounter)
			FreeImage_DeInitialise();
		#endif

		glDeleteTextures(1, &n);
	}

	void Texture::enable()
	{
		if (m_active != this)
		{
			glBindTexture(GL_TEXTURE_2D, n);
			m_active = this;
		}
	}

	Engine::Point<float> Texture::getTextureCoords()
	{
		return {
			static_cast<float>(getSourceWidth()) / static_cast<float>(getWidth()),
			static_cast<float>(getSourceHeight()) / static_cast<float>(getHeight())
		};
	}

	Engine::Texture* Texture::load(const char* filename)
	{
		Texture* texture = new Texture();

		FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename, 0);
		if (fif == FIF_UNKNOWN)
			fif = FreeImage_GetFIFFromFilename(filename);
		if (fif == FIF_UNKNOWN)
			return nullptr;
		if (!FreeImage_FIFSupportsReading(fif))
			return nullptr;
		FIBITMAP* dib = FreeImage_Load(fif, filename);

		BYTE* bits = FreeImage_GetBits(dib);

		texture->m_sourceWidth = FreeImage_GetWidth(dib);
		texture->m_sourceHeight = FreeImage_GetHeight(dib);

		const int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };
		int sizesCount = sizeof sizes / sizeof(int);
		unsigned int sourceSize = std::max<unsigned int>(texture->m_sourceWidth, texture->m_sourceHeight);
		int targetSize = sizes[sizesCount - 1];

		if (sourceSize > targetSize)
			return nullptr;

		for (int i = 0; i < sizesCount - 1; ++i)
			if (sourceSize <= sizes[i])
			{
				targetSize = sizes[i];
				break;
			}

		texture->m_size = targetSize;

		int size = texture->m_sourceWidth * texture->m_sourceHeight * 4;
		for (int i = 0; i < size; i += 4)
		{
			BYTE tmp = bits[i];
			bits[i] = bits[i + 2];
			bits[i + 2] = tmp;
		}

		BYTE* tex = new BYTE[targetSize * targetSize * 4];
		for (int j = 0, jj = 0; j < targetSize; ++j)
			for (int i = 0, ii = 0; i < targetSize; ++i)
			{
				if (j < texture->m_sourceHeight && i < texture->m_sourceWidth)
				{
					for (int k = 0; k < 4; ++k)
						tex[j * targetSize * 4 + i * 4 + k] = bits[jj * texture->m_sourceWidth * 4 + ii * 4 + k];

					if (++ii >= texture->m_sourceWidth)
					{
						ii = 0;
						++jj;
					}
				}
				else
				{
					for (int k = 0; k < 4; ++k)
						tex[j * targetSize * 4 + i * 4 + k] = static_cast<BYTE>(0);
				}
			}
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, targetSize, targetSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);

		FreeImage_Unload(dib);
		delete[] tex;

		return texture;
	}
}
