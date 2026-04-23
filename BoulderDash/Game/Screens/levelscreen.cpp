#include "levelscreen.h"

#include <algorithm>
#include <cstdio>
#include <sstream>
#include <ctime>

#include "gameover.h"
#include "timeIsUp.h"
#include "..\..\Engine\sprite.h"
#include "..\..\Engine\textureBase.h"
#include "..\..\Glut\freeglut.h"


namespace BoulderDash
{
	namespace Screens
	{
		LevelScreen::LevelScreen(Resources *resources, int levelId, int livesCount)
			: Screen(resources), m_levelId(levelId)
		{
			srand(time(nullptr));

			m_pauseScreen = 0;
			m_humanDirection = 2;
			m_humanMoving = 0;
			m_lives = livesCount;

			char cFileName[256];
			sprintf(cFileName, "Levels\\%d.txt", levelId);
			m_level = new Level(cFileName);

			m_totalCrystals = m_level->getCrystalsCount();
			m_crystals = 0;
			m_startPosition = m_level->findHumanPosition();
			m_time = m_level->getTime() * 1000;

			for (int j = 0; j < m_level->getHeight(); ++j)
				for (int i = 0; i < m_level->getWidth(); ++i)
				{
					CellType cell = static_cast<CellType>(m_level->get(i, j) & OBJECTS);
			
					if (cell == ENEMY0 || cell == ENEMY1)
					{
						Enemy* enemy = 0;

						if (cell == ENEMY0)
							enemy = new Enemies::Ghost(Engine::Point<int>(i, j));
						if (cell == ENEMY1)
							enemy = new Enemies::Dark(Engine::Point<int>(i, j));

						if (enemy != 0)
							m_enemies.push_back(enemy);
					}
				}

		}

		LevelScreen::~LevelScreen()
		{
			for (std::list<Enemy*>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
				delete *it;

			if (!m_humanMoving)
				delete m_humanMoving;
		}

		Screen* LevelScreen::tryMoveToCell(int xOffset, int yOffset)
		{
			Engine::Point<int> oldHumanPosition = m_level->findHumanPosition();
			Engine::Point<int> currHumanPosition = oldHumanPosition;
			CellType cell = m_level->get(oldHumanPosition.x + xOffset, oldHumanPosition.y + yOffset);

			if (canMoveToCell(cell))
			{
				bool doorVisible = calcCrystalsCount() == 0;
				if (cell == DOOR && doorVisible)
					return new LevelScreen(getResources(), m_levelId + 1, m_lives);

				if (cell == HEART)
					m_lives = std::min<int>(m_lives + 1, m_maxLives);

				if (cell == CRYSTAL)
					++m_crystals;

				if (cell == CLOCK)
					m_time += 10000;

				if (cell == ENEMY0 || cell == ENEMY1)
				{
					if (--m_lives)
					{
						m_level->set(
							m_startPosition.x,
							m_startPosition.y,
							static_cast<CellType>(m_level->get(m_startPosition.x, m_startPosition.y) | HUMAN));
						m_level->set(
							oldHumanPosition.x,
							oldHumanPosition.y,
							static_cast<CellType>(m_level->get(oldHumanPosition.x, oldHumanPosition.y) & GROUNDS));
						return 0;
					}
					else
					{
						return new GameOver(getResources());
					}
				}

				currHumanPosition = Engine::Point<int>(currHumanPosition.x + xOffset, oldHumanPosition.y + yOffset);
				const int humanMovingSpeed = 200;
				m_humanMoving = new Moving(
					oldHumanPosition,
					currHumanPosition,
					humanMovingSpeed);

				if (xOffset == 0 && yOffset < 0)
					m_humanDirection = 0;
				else if (xOffset > 0 && yOffset == 0)
					m_humanDirection = 1;
				else if (xOffset == 0 && yOffset > 0)
					m_humanDirection = 2;
				else if (xOffset < 0 && yOffset == 0)
					m_humanDirection = 3;
				else
					m_humanDirection = 2;
			}

			m_level->set(oldHumanPosition.x, oldHumanPosition.y, NONE);
			m_level->set(currHumanPosition.x, currHumanPosition.y, HUMAN);
			return 0;
		}

		void LevelScreen::save()
		{
			save("save.txt");
		}

		Screen* LevelScreen::update(int timeSpan, std::list<int> kbdEvents, MouseState mouseState)
		{
			if (!m_pauseScreen)
				for (std::list<int>::iterator it = kbdEvents.begin(); it != kbdEvents.end(); ++it)
					if (*it == 27 /* Escape */)
					{
						m_pauseScreen = new PauseScreen(getResources(), this, static_cast<void (Screen::*)()>(&LevelScreen::save));
						break;
					}

			if (m_pauseScreen)
			{
				m_pauseScreen = m_pauseScreen->update(timeSpan, kbdEvents, mouseState);
				return 0;
			}

			if ((m_time -= timeSpan) <= 0)
				return new TimeIsUp(getResources());

			getResources()->getAnimation("crystal", 21)->update(timeSpan);
			getResources()->getAnimation("heart", 21)->update(timeSpan);

			if (m_humanMoving)
			{
				m_humanMoving->update(timeSpan);

				if (m_humanMoving->isDone())
				{
					delete m_humanMoving;
					m_humanMoving = 0;
				}
			}

			Screen* levelScreen = 0;
			if (!m_humanMoving && kbdEvents.size() > 0)
			{
				switch (kbdEvents.back())
				{
				case GLUT_KEY_UP:
					levelScreen = tryMoveToCell(0, -1);
					break;

				case GLUT_KEY_RIGHT:
					levelScreen = tryMoveToCell(1, 0);
					break;

				case GLUT_KEY_DOWN:
					levelScreen = tryMoveToCell(0, 1);
					break;

				case GLUT_KEY_LEFT:
					levelScreen = tryMoveToCell(-1, 0);
					break;
				}
			}
			kbdEvents.clear();

			bool humanAnimationUpdateNotNeeded = false;

			if (levelScreen)
				return levelScreen;

			for (std::list<Enemy*>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
			{
				PointsPair pair = (*it)->update(m_level, timeSpan);
				Engine::Point<int> oldPoint = pair.first;
				Engine::Point<int> targetPoint = pair.second;

				if (oldPoint.x != targetPoint.x || oldPoint.y != targetPoint.y)
				{
					if (canMoveEnemyToCell(m_level->get(targetPoint.x, targetPoint.y)))
					{
						if ((m_level->get(targetPoint) & OBJECTS) == HUMAN)
						{
							if (--m_lives)
							{
								m_level->set(
									m_startPosition.x,
									m_startPosition.y,
									static_cast<CellType>(m_level->get(m_startPosition.x, m_startPosition.y) | HUMAN));
							}
							else
							{
								return new GameOver(getResources());
							}
						}

						m_level->set(targetPoint.x, targetPoint.y,
							static_cast<CellType>(m_level->get(oldPoint.x, oldPoint.y) & OBJECTS | m_level->get(targetPoint.x, targetPoint.y) & GROUNDS));
						m_level->set(oldPoint.x, oldPoint.y,
							static_cast<CellType>(m_level->get(oldPoint.x, oldPoint.y) & ~OBJECTS));

						m_enemyMovings.push_back(Moving(oldPoint, targetPoint, 1000));
						(*it)->setLocation(targetPoint.x, targetPoint.y);
					}
				}
			}

			for (std::list<Moving>::iterator it = m_movings.begin(); it != m_movings.end(); ++it)
				it->update(timeSpan);
			for (std::list<Moving>::iterator it = m_enemyMovings.begin(); it != m_enemyMovings.end(); ++it)
				it->update(timeSpan);

			Engine::Point<int> humanPosition = m_level->findHumanPosition();

			for (int i = 0; i < m_level->getWidth(); ++i)
				for (int j = m_level->getHeight() - 2; j >= 0; --j)
				{
					int currObject = m_level->get(i, j) & OBJECTS;
					int bottomCell = m_level->get(i, j + 1);

					bool moved = false;
					bool movingDone = false;
					for (std::list<Moving>::iterator it = m_movings.begin(); it != m_movings.end() && !moved; ++it)
						if (it->contains(i, j))
						{
							moved = true;
							movingDone = it->isDone();
						}

					if ((currObject == BOULDER || currObject == CRYSTAL) && bottomCell == NONE ||
						currObject == BOULDER && moved && movingDone && (bottomCell & OBJECTS) == HUMAN)
					{
						if (currObject == BOULDER && humanPosition.x == i && humanPosition.y == j + 1)
							if (--m_lives)
							{
								m_level->set(
									m_startPosition.x,
									m_startPosition.y,
									static_cast<CellType>(m_level->get(m_startPosition.x, m_startPosition.y) | HUMAN));
							}
							else
							{
								return new GameOver(getResources());
							}

						if (!moved || movingDone)
						{
							m_level->set(i, j + 1,
								static_cast<CellType>(m_level->get(i, j) & OBJECTS | m_level->get(i, j + 1) & GROUNDS));
							m_level->set(i, j,
								static_cast<CellType>(m_level->get(i, j) & ~OBJECTS));
							Moving moving(Engine::Point<int>(i, j), Engine::Point<int>(i, j + 1), 300);
							m_movings.push_back(moving);
						}
					}
				}

			m_movings.remove_if([](Moving moving) { return moving.isDone(); });
			m_enemyMovings.remove_if([](Moving moving) { return moving.isDone(); });

			bool doorVisible = calcCrystalsCount() == 0;
			if (doorVisible)
				getResources()->getAnimation("door", 21)->update(timeSpan);

			if (m_humanMoving && !humanAnimationUpdateNotNeeded)
				getHumanAnimation()->update(timeSpan);

			return 0;
		}

		void LevelScreen::draw(int timeSpan, int width, int height)
		{
			if (m_pauseScreen)
			{
				m_pauseScreen->draw(timeSpan, width, height);
				return;
			}

			int optimalRectSize = std::min<int>(
				width / Level::minCellsOnScreen,
				height / Level::minCellsOnScreen);

			Engine::Point<int> humanPosition = m_level->findHumanPosition();

			std::pair<int, int> humanMovingOffset;
			if (m_humanMoving)
				humanMovingOffset = std::pair<int, int>(
					-(m_humanMoving->target().x - m_humanMoving->old().x) * optimalRectSize * m_humanMoving->donePercents(),
					(m_humanMoving->target().y - m_humanMoving->old().y) * optimalRectSize * m_humanMoving->donePercents());

			float xOffsetMin = static_cast<float>(width) / static_cast<float>(optimalRectSize) - static_cast<float>(m_level->getWidth());
			float xOffsetMax = 0;

			float xOffset = width / static_cast<float>(optimalRectSize) / 2.f - humanPosition.x - humanMovingOffset.first / static_cast<float>(optimalRectSize);
			xOffset = std::min<float>(xOffset, xOffsetMax);
			xOffset = std::max<float>(xOffset, xOffsetMin);

			float yOffsetMin = height / optimalRectSize - m_level->getHeight();
			float yOffsetMax = 0;
		
			float yOffset = humanPosition.y - m_level->getHeight() + height / static_cast<float>(optimalRectSize) / 2.f - humanMovingOffset.second / static_cast<float>(optimalRectSize);
			yOffset = std::min<float>(yOffset, yOffsetMax);
			yOffset = std::max<float>(yOffset, yOffsetMin);

			xOffset *= optimalRectSize;
			yOffset *= optimalRectSize;

			bool doorVisible = calcCrystalsCount() == 0;

			for (int j = 0; j < m_level->getHeight(); ++j)
				for (int i = 0; i < m_level->getWidth(); ++i)
				{
					CellType cellType = m_level->get(i, j);
					Engine::TextureBase* tex = 0;
				
					switch (cellType)
					{
					case GROUND:
						tex = getResources()->getTexture("ground");
						break;

					case BOX:
						tex = getResources()->getTexture("box");
						break;

					case NONE:
					default:
						tex = getResources()->getTexture("bg");
						break;
					}

					if (tex)
					{
						Engine::Sprite sprite(tex);
						sprite.draw(
							xOffset + i * optimalRectSize,
							yOffset + (m_level->getHeight() - j - 1) * optimalRectSize,
							optimalRectSize,
							optimalRectSize);
					}
				}

			for (int j = 0; j < m_level->getHeight(); ++j)
				for (int i = 0; i < m_level->getWidth(); ++i)
				{
					CellType cellType = m_level->get(i, j);

					if (!(cellType & OBJECTS))
						continue;

					std::pair<int, int> movingOffset;
					int enemyDirection = 2;

					if (cellType == HUMAN && m_humanMoving)
					{
						movingOffset = humanMovingOffset;
					}
					else if (cellType == ENEMY0 || cellType == ENEMY1)
					{
						for (std::list<Moving>::iterator it = m_enemyMovings.begin(); it != m_enemyMovings.end(); ++it)
							if (it->contains(i, j))
							{
								movingOffset = std::pair<int, int>(
									-(it->target().x - it->old().x) * optimalRectSize * it->donePercents(),
									(it->target().y - it->old().y) * optimalRectSize * it->donePercents());
								break;
							}

						for (std::list<Enemy*>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
						{
							Engine::Point<int> pt = (*it)->getLocation();
							if (pt.x == i && pt.y == j)
							{
								enemyDirection = (*it)->getDirection();
								break;
							}
						}
					}
					else
						for (std::list<Moving>::iterator it = m_movings.begin(); it != m_movings.end(); ++it)
							if (it->contains(i, j))
							{
								movingOffset = std::pair<int, int>(
									(it->target().x - it->old().x) * optimalRectSize * it->donePercents(),
									(it->target().y - it->old().y) * optimalRectSize * it->donePercents());
								break;
							}

					Engine::TextureBase* tex = 0;

					switch (cellType)
					{
					case HUMAN:
						tex = getHumanAnimation();
						break;

					case CRYSTAL:
						tex = getResources()->getAnimation("crystal", 21);
						break;

					case BOULDER:
						tex = getResources()->getTexture("boulder");
						break;

					case ENEMY0:
						tex = getEnemyAnimation(0, enemyDirection);
						break;

					case ENEMY1:
						tex = getEnemyAnimation(1, enemyDirection);
						break;

					case HEART:
						tex = getResources()->getAnimation("heart", 21);
						break;

					case CLOCK:
						tex = getResources()->getTexture("clock");
						break;

					case DOOR:
						if (doorVisible)
							tex = getResources()->getAnimation("door", 21);
						else
							tex = getResources()->getTexture("ground");
						break;

					default:
						continue;
					}

					if (tex)
					{
						Engine::Sprite sprite(tex);
						sprite.draw(
							xOffset + i * optimalRectSize + movingOffset.first,
							yOffset + (m_level->getHeight() - j - 1) * optimalRectSize + movingOffset.second,
							optimalRectSize,
							optimalRectSize);
					}
				}

			drawPanel(width, height);
		}

		void LevelScreen::drawPanel(int width, int height)
		{
			int panelHeight = height / 20.f;
			Engine::Sprite panelSprite(getResources()->getTexture("pnl"));
			panelSprite.draw(0, height - panelHeight, width, panelHeight);

			Engine::Sprite heartSprite(getResources()->getTexture("heart"));
			Engine::Sprite emptyHeartSprite(getResources()->getTexture("emptyHeart"));
			Engine::Sprite* currHeartSprite;
			float heartSize = panelHeight;
			float heartDist = heartSize * 0.1f;
			float heartX = 0;
			float heartY = height - heartSize;
			for (int i = 0; i < m_maxLives; ++i)
			{
				currHeartSprite = i < m_lives ? &heartSprite : &emptyHeartSprite;
				currHeartSprite->draw(heartX, heartY, heartSize, heartSize);
				heartX += heartSize + heartDist;
			}

			Engine::Sprite crystalSprite(getResources()->getTexture("crystal"));
			float crystalSize = panelHeight;
			float crystalDist = crystalSize * 0.1f;
			float crystalX = heartX + crystalSize * 2.f;
			float crystalY = height - crystalSize;
			crystalSprite.draw(crystalX, crystalY, crystalSize, crystalSize);
			crystalX += crystalSize * 1.3f;
			crystalSize *= 0.5f;
			crystalY = height - panelHeight + (panelHeight - crystalSize) / 2.f;

			std::list<int> num;
			int crystalsCount = m_crystals;
			if (crystalsCount > 0)
			{
				while (crystalsCount > 0)
				{
					num.push_front(crystalsCount % 10);
					crystalsCount /= 10;
				}
			}
			else
				num.push_front(0);
			for (std::list<int>::iterator it = num.begin(); it != num.end(); ++it)
			{
				Engine::Texture* tex = getResources()->getAnimation("num", 10)->getFrame(*it);
				Engine::Sprite numSprite(tex);
				float height = crystalSize;
				float width = tex->getSourceWidth() / static_cast<float>(tex->getSourceHeight())  * height;
				numSprite.draw(crystalX, crystalY, width, height);
				crystalX += width * 1.3f;
			}

			Engine::Texture *slashTexture = getResources()->getTexture("num", "slash");
			Engine::Sprite slashSprite(slashTexture);
			float slashHeight = crystalSize;
			float slashWidth = slashTexture->getSourceWidth() / static_cast<float>(slashTexture->getSourceHeight())  * slashHeight;
			crystalX += slashWidth * 0.1f;
			slashSprite.draw(crystalX, crystalY, slashWidth, slashHeight);
			crystalX += slashWidth * 1.1f;

			std::list<int> totalNum;
			int totalCrystalsCount = m_totalCrystals;
			if (totalCrystalsCount > 0)
			{
				while (totalCrystalsCount > 0)
				{
					totalNum.push_front(totalCrystalsCount % 10);
					totalCrystalsCount /= 10;
				}
			}
			else
				totalNum.push_front(0);
			for (std::list<int>::iterator it = totalNum.begin(); it != totalNum.end(); ++it)
			{
				Engine::Texture* tex = getResources()->getAnimation("num", 10)->getFrame(*it);
				Engine::Sprite numSprite(tex);
				float height = crystalSize;
				float width = tex->getSourceWidth() / static_cast<float>(tex->getSourceHeight())  * height;
				numSprite.draw(crystalX, crystalY, width, height);
				crystalX += width * 1.3f;
			}

			Engine::Texture *clockTexture = getResources()->getTexture("clock");
			Engine::Sprite clockSprite(clockTexture);
			float clockHeight = panelHeight;
			float clockWidth = clockTexture->getSourceWidth() / static_cast<float>(clockTexture->getSourceHeight())  * clockHeight;
			float clockX = crystalX + 50;
			float clockY = height - panelHeight;
			clockSprite.draw(clockX, clockY, clockWidth, clockHeight);

			std::list<int> timeNum;
			int time = m_time / 1000;
			if (time > 0)
			{
				while (time > 0)
				{
					timeNum.push_front(time % 10);
					time /= 10;
				}
			}
			else
				timeNum.push_front(0);
			clockX += 50;
			for (std::list<int>::iterator it = timeNum.begin(); it != timeNum.end(); ++it)
			{
				Engine::Texture* tex = getResources()->getAnimation("num", 10)->getFrame(*it);
				Engine::Sprite numSprite(tex);
				float height = crystalSize;
				float width = tex->getSourceWidth() / static_cast<float>(tex->getSourceHeight())  * height;
				numSprite.draw(clockX, crystalY, width, height);
				clockX += width * 1.3f;
			}
		}

		int LevelScreen::calcCrystalsCount() const
		{
			if (!m_level)
				return 0;

			int count = 0;
			for (int j = 0; j < m_level->getHeight(); ++j)
				for (int i = 0; i < m_level->getWidth(); ++i)
					if ((m_level->get(i, j) & OBJECTS) == CRYSTAL)
								++count;

			return count;
		}

		void LevelScreen::save(const char* fileName)
		{
			FILE *file = fopen(fileName, "w");

			fprintf(file, "%d %d %d %d %d\n", m_levelId, m_lives, m_time, m_crystals, m_totalCrystals);
			for (int j = 0; j < m_level->getHeight(); ++j)
				for (int i = 0; i < m_level->getWidth(); ++i)
				{
					fprintf(file, "%d ", m_level->get(i, j));
				}

			fclose(file);
		}

		LevelScreen* LevelScreen::load(Resources *resources, const char* fileName)
		{
			FILE *file = fopen(fileName, "r");
			if (!file)
				return new LevelScreen(resources, 1);

			int levelId, livesCount, time, crystals, totalCrystals;
			fscanf(file, "%d %d %d %d %d\n", &levelId, &livesCount, &time, &crystals, &totalCrystals);
			LevelScreen* levelScreen = new LevelScreen(resources, levelId, livesCount);
			for (int j = 0; j < levelScreen->m_level->getHeight(); ++j)
				for (int i = 0; i < levelScreen->m_level->getWidth(); ++i)
				{
					int cell;
					fscanf(file, "%d", &cell);
					levelScreen->m_level->set(i, j, static_cast<CellType>(cell));
				}

			fclose(file);
			return levelScreen;
		}

		Engine::Animation* LevelScreen::getHumanAnimation()
		{
			std::stringstream direction;
			direction << m_humanDirection;
			return getResources()->getAnimation("human", direction.str(), 3);
		}

		Engine::Animation* LevelScreen::getEnemyAnimation(int enemy, int direction)
		{
			std::stringstream path;
			path << "enemy" << enemy;
			std::stringstream dir;
			dir << direction;
			return getResources()->getAnimation(path.str(), dir.str(), 3);
		}

		int LevelScreen::m_maxLives = 3;
	}
}