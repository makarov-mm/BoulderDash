#include "resources.h"
#include "constants.h"

#include <filesystem>
#include <format>
#include <string_view>

namespace BoulderDash
{
	namespace
	{
		std::filesystem::path gfx(std::string_view name)
		{
			std::filesystem::path path = std::filesystem::path("Gfx") / name;
			return path.make_preferred();
		}

		Engine::Texture image(std::string_view name)
		{
			return Engine::Texture(gfx(name) += ".png");
		}

		Engine::Animation animation(std::string_view name, int frames)
		{
			return Engine::Animation(gfx(name), frames, animationFps);
		}

		// Walking in each Direction: Gfx/<name>/<direction>/frame0-2.png
		std::array<Engine::Animation, 4> walking(std::string_view name)
		{
			const auto walk = [&](int direction) { return animation(std::format("{}/{}", name, direction), 3); };
			return { walk(0), walk(1), walk(2), walk(3) };
		}
	}

	Resources::Resources()
		: background(image("bg")),
		  ground(image("ground")),
		  box(image("box")),
		  boulder(image("boulder")),
		  clock(image("clock")),
		  crystalAnimation(animation("crystal", 21)),
		  heartAnimation(animation("heart", 21)),
		  doorAnimation(animation("door", 21)),
		  human(walking("human")),
		  ghost(walking("enemy0")),
		  dark(walking("enemy1")),
		  panel(image("pnl")),
		  heart(image("heart")),
		  emptyHeart(image("emptyHeart")),
		  crystal(image("crystal")),
		  slash(image("num/slash")),
		  digits(animation("num", 10)),
		  title(image("title")),
		  pause(image("pause")),
		  gameOver(image("gameOver")),
		  timeIsUp(image("timeIsUp")),
		  newGame(image("buttons/newGame")),
		  newGameHovered(image("buttons/newGameActive")),
		  load(image("buttons/load")),
		  loadHovered(image("buttons/loadActive")),
		  resume(image("buttons/resume")),
		  resumeHovered(image("buttons/resumeActive")),
		  save(image("buttons/save")),
		  saveHovered(image("buttons/saveActive"))
	{
	}
}
