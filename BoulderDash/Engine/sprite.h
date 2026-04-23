#ifndef SPRITE_H
#define SPRITE_H

#include "texture.h"

namespace Engine
{
	class Sprite
	{
	private:
		TextureBase* m_tex;
	public:
		Sprite(TextureBase *tex);
		void draw(float x, float y, float width, float height) const;
		TextureBase* getTexture() const { return m_tex; }
	};
}

#endif
