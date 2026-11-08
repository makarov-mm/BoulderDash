#pragma once

#include "resources.h"
#include "screen.h"
#include "../Engine/gameBase.h"

#include <memory>

namespace BoulderDash
{
	class Game final : public Engine::GameBase
	{
	public:
		Game();

	private:
		void update(int time, const Engine::Input& input) override;
		void draw(int width, int height) override;

		Resources m_resources;
		std::unique_ptr<Screen> m_screen;
	};
}
