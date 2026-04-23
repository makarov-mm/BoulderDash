#ifndef GAMEBASE_H
#define GAMEBASE_H

#include <windows.h>
#include <list>

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")
#pragma comment (lib, "Winmm.lib")
#pragma comment (lib, "freeglut.lib")

namespace Engine
{
	class GameBase
	{
	private:
		int m_wnd;
		int m_width, m_height;
		DWORD m_prevTime;
		int m_mouseX, m_mouseY;
		bool m_mouseLeft, m_mouseRight;
		friend void mouseMotion(int x, int y);
		friend void mouse(int button, int state, int x, int y);
	public:
		GameBase(const char* title);
		virtual ~GameBase();

		void run();
		void resize(int width, int height);
		std::list<int> kbdState;
		DWORD getPrevTime() const { return m_prevTime; }
		void updateTime(DWORD timeSpan) { m_prevTime += timeSpan; }

		virtual void load() { };
		virtual void unload() { };
		virtual void update(int timeSpan) { };
		virtual void draw(int timeSpan) { };

		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }

		int getMouseX() const { return m_mouseX; }
		int getMouseY() const { return m_mouseY; }

		bool getMouseLeftButton() const { return m_mouseLeft; }
		bool getMouseRightButton() const { return m_mouseRight; }
	};
}

#endif
