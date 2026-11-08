#pragma once

#include "../resources.h"
#include "../ui.h"

namespace BoulderDash::Screens
{
	// The pause menu, shown over a level by the level itself.
	class PauseScreen
	{
	public:
		enum class Choice { None, Resume, Save };

		explicit PauseScreen(const Resources& resources);

		Choice update(const Engine::Input& input);
		void draw(int width, int height) const;

	private:
		ButtonMenu m_menu;
	};
}
