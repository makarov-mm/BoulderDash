#pragma once

#include "../screen.h"
#include "../ui.h"

namespace BoulderDash::Screens
{
	class MenuScreen final : public Screen
	{
	public:
		explicit MenuScreen(const Resources& resources);

		std::unique_ptr<Screen> update(int time, const Engine::Input& input) override;
		void draw(int width, int height) const override;

	private:
		ButtonMenu m_menu;
	};
}
