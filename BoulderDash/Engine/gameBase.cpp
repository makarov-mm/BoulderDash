#include "gameBase.h"
#include <map>
#include <algorithm>
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "..\Glut\freeglut.h"

namespace Engine
{
	static std::map<int, GameBase*> games;

	void display()
	{
		int currentWindow = glutGetWindow();
		DWORD timeSpan = GetTickCount() - games[currentWindow]->getPrevTime();

		if (timeSpan > 0)
		{
			games[currentWindow]->updateTime(timeSpan);
			games[currentWindow]->update(timeSpan);
			glClear(GL_COLOR_BUFFER_BIT);
			glLoadIdentity();
			games[currentWindow]->draw(timeSpan);
			glutSwapBuffers();
		}
	}

	void idle()
	{
		display();
	}

	void kbdSpec(int key, int x, int y)
	{
		int currentWindow = glutGetWindow();
		if (std::find(games[currentWindow]->kbdState.begin(), games[currentWindow]->kbdState.end(), key) == games[currentWindow]->kbdState.end())
			games[currentWindow]->kbdState.push_back(key);
	}

	void mouse(int button, int state, int x, int y)
	{
		int currentWindow = glutGetWindow();

		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			games[currentWindow]->m_mouseLeft = state == GLUT_DOWN;
			break;

		case GLUT_RIGHT_BUTTON:
			games[currentWindow]->m_mouseRight = state == GLUT_DOWN;
			break;
		}
	}

	void mouseMotion(int x, int y)
	{
		int currentWindow = glutGetWindow();
		games[currentWindow]->m_mouseX = x;
		games[currentWindow]->m_mouseY = y;
	}

	void kbdSpecUp(int key, int x, int y)
	{
		int currentWindow = glutGetWindow();
		games[currentWindow]->kbdState.erase(
			std::remove(games[currentWindow]->kbdState.begin(), games[currentWindow]->kbdState.end(), key),
			games[currentWindow]->kbdState.end());
	}

	void kbd(unsigned char key, int x, int y)
	{
		kbdSpec(key, x, y);
	}

	void kbdUp(unsigned char key, int x, int y)
	{
		kbdSpecUp(key, x, y);
	}

	void reshape(int width, int height)
	{
		int currentWindow = glutGetWindow();
		games[currentWindow]->resize(width, height);
	}

	GameBase::GameBase(const char* title)
	{
		m_mouseX = m_mouseY = 0;
		m_mouseLeft = m_mouseRight = false;

		int count = 0;
		char* str = "";
		glutInit(&count, &str);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

		int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
		int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
		constexpr int windowWidth = 800;
		constexpr int windowHeight = 600;
		glutInitWindowPosition((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);
		glutInitWindowSize(windowWidth, windowHeight);
		glutSetWindow(m_wnd = glutCreateWindow(title));

		glutDisplayFunc(display);
		glutIdleFunc(idle);
		glutKeyboardFunc(kbd);
		glutKeyboardUpFunc(kbdUp);
		glutSpecialFunc(kbdSpec);
		glutSpecialUpFunc(kbdSpecUp);
		glutMotionFunc(mouseMotion);
		glutMouseFunc(mouse);
		glutPassiveMotionFunc(mouseMotion);
		glutReshapeFunc(reshape);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		games.insert(std::pair<int, GameBase*>(m_wnd, this));
	}

	GameBase::~GameBase()
	{
		games.erase(m_wnd);
	}

	void GameBase::run()
	{
		load();
		glutMainLoop();
		unload();
	}

	void GameBase::resize(int width, int height)
	{
		glViewport(0, 0, m_width = width, m_height = height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}
