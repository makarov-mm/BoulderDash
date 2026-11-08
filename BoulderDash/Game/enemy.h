#pragma once

#include "cellType.h"
#include "constants.h"
#include "direction.h"
#include "level.h"
#include "moving.h"

#include <optional>
#include <random>

namespace BoulderDash
{
	// A ghost or a dark one. Once a second it steps towards the human along the shortest free path,
	// or in a random direction when there is none.
	class Enemy
	{
	public:
		Enemy(Object kind, Engine::Point<int> position) : m_kind(kind), m_position(position) { }

		// Enemies walk through empty cells only, and into the human.
		static bool canEnter(Cell cell)
		{
			return cell.tile == Tile::Empty && (cell.object == Object::None || cell.object == Object::Human);
		}

		Object kind() const { return m_kind; }
		Engine::Point<int> position() const { return m_position; }
		Direction direction() const { return m_direction; }
		const std::optional<Moving>& moving() const { return m_moving; }
		int walkTime() const { return m_walkTime; } // for the walking animation

		// Advances the enemy by `time` ms; returns the cell it wants to step into, when it is time to step.
		std::optional<Engine::Point<int>> update(int time, const Level& level, Engine::Point<int> human, std::mt19937& random);

		// Steps into the next cell.
		void moveTo(Engine::Point<int> cell);

	private:
		Direction directionTo(const Level& level, Engine::Point<int> human, std::mt19937& random) const;

		Object m_kind;
		Engine::Point<int> m_position;
		Direction m_direction = Direction::Down;
		std::optional<Moving> m_moving;
		int m_stepTimer = enemyStepTime; // ms
		int m_walkTime = 0;
	};
}
