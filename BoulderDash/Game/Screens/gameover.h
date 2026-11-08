#pragma once

#include "../screen.h"

namespace BoulderDash::Screens
{
	// "Game over" or "Time is up"; Enter (or Escape, Space, a click) goes back to the menu.
	class GameOver final : public Screen
	{
	public:
		enum class Reason { NoLives, TimeIsUp };

		GameOver(const Resources& resources, Reason reason);

		std::unique_ptr<Screen> update(int time, const Engine::Input& input) override;
		void draw(int width, int height) const override;

	private:
		const Engine::Texture& m_banner;
	};
}
