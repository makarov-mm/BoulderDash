#pragma once

#include "../Engine/animation.h"
#include "../Engine/texture.h"

#include <array>

namespace BoulderDash
{
	// The images of the game, all loaded from Gfx at start.
	struct Resources
	{
		Resources();

		// Cells
		Engine::Texture background, ground, box, boulder, clock;
		Engine::Animation crystalAnimation, heartAnimation, doorAnimation;
		std::array<Engine::Animation, 4> human, ghost, dark; // by Direction

		// The panel
		Engine::Texture panel, heart, emptyHeart, crystal, slash;
		Engine::Animation digits;

		// Menus
		Engine::Texture title, pause, gameOver, timeIsUp;
		Engine::Texture newGame, newGameHovered, load, loadHovered, resume, resumeHovered, save, saveHovered;
	};
}
