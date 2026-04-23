#ifndef TEXTUREBASE_H
#define TEXTUREBASE_H

#include "point.h"

namespace Engine
{
	class TextureBase
	{
	public:
		virtual ~TextureBase() = default;
		virtual void enable() = 0;
		virtual Point<float> getTextureCoords() = 0;
	};
}

#endif
