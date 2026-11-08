#pragma once

#include "pausescreen.h"
#include "../constants.h"
#include "../direction.h"
#include "../enemy.h"
#include "../level.h"
#include "../moving.h"
#include "../screen.h"

#include <istream>
#include <optional>
#include <random>
#include <vector>

namespace BoulderDash::Screens
{
	// A level being played.
	class LevelScreen final : public Screen
	{
	public:
		// Starts level `id`; throws if its file cannot be read.
		LevelScreen(const Resources& resources, int id, int lives = startLives);

		// Restores the saved game, or starts a new one if there is no valid save.
		static std::unique_ptr<LevelScreen> load(const Resources& resources);

		std::unique_ptr<Screen> update(int time, const Engine::Input& input) override;
		void draw(int width, int height) const override;

	private:
		bool restore(std::istream& in, int time, int crystals);
		void save() const;
		void findActors(); // the human and the enemies in the level

		bool humanCanEnter(Cell cell) const;
		std::unique_ptr<Screen> moveHuman(Direction direction);
		std::unique_ptr<Screen> moveEnemies(int time);
		std::unique_ptr<Screen> dropObjects(int time);
		std::unique_ptr<Screen> loseLife();
		std::unique_ptr<Screen> nextLevel() const;
		Engine::Point<int> freeCellNear(Engine::Point<int> cell) const;

		const Engine::Texture* objectTexture(Object object) const;
		void drawPanel(int width, int height) const;
		float drawNumber(int number, float x, float y, float height) const;

		int m_id;
		Level m_level;
		int m_lives;
		int m_timeLeft; // ms
		int m_crystals = 0;
		int m_totalCrystals;
		bool m_doorOpen;
		int m_clock = 0;     // ms played, for the animations
		int m_doorClock = 0; // ms since the door opened

		Engine::Point<int> m_start; // where the human starts, and comes back after losing a life
		Engine::Point<int> m_human;
		Direction m_humanDirection = Direction::Down;
		std::optional<Moving> m_humanMoving;
		int m_humanWalkTime = 0;

		std::vector<Enemy> m_enemies;
		std::vector<Moving> m_falling; // boulders and crystals
		std::optional<PauseScreen> m_pause;
		std::mt19937 m_random{ std::random_device{}() };
	};
}
