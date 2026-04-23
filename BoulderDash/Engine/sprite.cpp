#include "sprite.h"

#include <windows.h>
#include <gl/gl.h>

namespace Engine
{
	Sprite::Sprite(TextureBase* tex)
	{
		m_tex = tex;
	}

	void Sprite::draw(float x, float y, float width, float height) const
	{
		m_tex->enable();
		Point<float> texCoord = m_tex->getTextureCoords();

		glBegin(GL_TRIANGLES);
		glColor4f(1.f, 1.f, 1.f, 1.f);

		glTexCoord2f(0.f, texCoord.y);
		glVertex3f(x, y + height, 0.f);

		glTexCoord2f(0.f, 0.f);
		glVertex3f(x, y, 0.f);

		glTexCoord2f(texCoord.x, 0.f);
		glVertex3f(x + width, y, 0.f);

		glTexCoord2f(0.f, texCoord.y);
		glVertex3f(x, y + height, 0.f);

		glTexCoord2f(texCoord.x, 0.f);
		glVertex3f(x + width, y, 0.f);

		glTexCoord2f(texCoord.x, texCoord.y);
		glVertex3f(x + width, y + height, 0.f);

		glEnd();
	}
}
