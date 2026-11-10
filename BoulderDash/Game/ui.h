#pragma once

#include "../Engine/input.h"
#include "../Engine/texture.h"

#include <optional>
#include <vector>

namespace BoulderDash
{
	// Covers the window with tiles of the size of a cell.
	void drawTiles(const Engine::Texture& tile, int width, int height);

	// A title with buttons under it, over a tiled background: the main and the pause menu.
	class ButtonMenu
	{
	public:
		struct Button
		{
			const Engine::Texture& normal;
			const Engine::Texture& hovered;
		};

		// The title takes `titleShare` of the window width; the buttons are scaled as much as the title.
		ButtonMenu(const Engine::Texture& background, const Engine::Texture& title, float titleShare, std::vector<Button> buttons);

		// The index of the button clicked, if any.
		std::optional<std::size_t> update(const Engine::Input& input);
		void draw(int width, int height) const;

	private:
		// The title, then the buttons
		std::vector<Engine::Rect> layout(int width, int height) const;

		const Engine::Texture& m_background;
		const Engine::Texture& m_title;
		float m_titleShare;
		std::vector<Button> m_buttons;
		std::optional<std::size_t> m_hovered;
	};
}
