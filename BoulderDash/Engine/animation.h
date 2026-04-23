#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

#include "textureBase.h"
#include "texture.h"

namespace Engine
{
	class Animation final: public TextureBase
	{
	private:
		std::vector<Texture*> m_textures;
		int m_frameTime;
		int m_timeSpan;
		int m_frame;
	public:
		Animation(std::vector<Texture*> textures, int framesPerSecond)
			: m_textures(textures.begin(), textures.end()), m_frameTime(1000 / framesPerSecond), m_timeSpan(0), m_frame(0)
		{ }

		~Animation() override;
		void update(int timeSpan);
		void reset();
		Texture* current() const { return getFrame(m_frame); }
		Texture* getFrame(int n) const { return m_textures[n]; }
		void enable() override { current()->enable(); }
		Point<float> getTextureCoords() override { return current()->getTextureCoords(); }
	};
}

#endif
