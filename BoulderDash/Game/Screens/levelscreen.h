#ifndef LEVELSCREEN_H
#define LEVELSCREEN_H

#include "..\screen.h"
#include "..\level.h"
#include "..\moving.h"
#include <list>
#include <utility>
#include "..\..\Engine\texture.h"
#include "..\..\Engine\point.h"
#include "..\enemy.h"
#include "..\Enemies\ghost.h"
#include "..\Enemies\dark.h"
#include <cstdio>
#include "pausescreen.h"

namespace BoulderDash
{
	namespace Screens
	{
		class LevelScreen final: public Screen
		{
		private:
			Level* m_level;
			std::list<Moving> m_movings;
			std::list<Moving> m_enemyMovings;
			std::list<Enemy*> m_enemies;
			Moving* m_humanMoving;
			Engine::Point<int> m_startPosition;
			int m_levelId;
			int calcCrystalsCount() const;
			int m_humanDirection;
			int m_lives;
			int m_time;
			static int m_maxLives;
			int m_crystals;
			int m_totalCrystals;
			void drawPanel(int width, int height);
			Screen* tryMoveToCell(int xOffset, int yOffset);
			Engine::Animation* getHumanAnimation();
			Engine::Animation* getEnemyAnimation(int enemy, int direction);
			Screen* m_pauseScreen;
			void save();
		public:
			LevelScreen(Resources *resources, int levelId, int livesCount = 2);
			~LevelScreen() override;

			virtual Screen* update(int timeSpan, std::list<int> kbdEvents, MouseState mouseState);
			virtual void draw(int timeSpan, int width, int height);
			int getLevelId() const { return m_levelId; }

			void save(const char* fileName);
			static LevelScreen* load(Resources *resources, const char* fileName);
		};
	}
}

#endif