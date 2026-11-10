#include "ui.h"
#include "constants.h"

#include <utility>

namespace BoulderDash
{
	void drawTiles(const Engine::Texture& tile, int width, int height)
	{
		const int size = cellSize(width, height);

		if (size <= 0)
			return;

		for (int y = 0; y < height; y += size)
			for (int x = 0; x < width; x += size)
				tile.draw(static_cast<float>(x), static_cast<float>(y), static_cast<float>(size), static_cast<float>(size));
	}

	ButtonMenu::ButtonMenu(const Engine::Texture& background, const Engine::Texture& title, float titleShare, std::vector<Button> buttons)
		: m_background(background), m_title(title), m_titleShare(titleShare), m_buttons(std::move(buttons))
	{ }

	std::optional<std::size_t> ButtonMenu::update(const Engine::Input& input)
	{
		const std::vector<Engine::Rect> rects = layout(input.width, input.height);
		m_hovered.reset();

		for (std::size_t i = 0; i < m_buttons.size(); ++i)
			if (rects[i + 1].contains(input.mouse))
				m_hovered = i;

		return input.clicked ? m_hovered : std::nullopt;
	}

	void ButtonMenu::draw(int width, int height) const
	{
		drawTiles(m_background, width, height);
		const std::vector<Engine::Rect> rects = layout(width, height);
		m_title.draw(rects[0]);

		for (std::size_t i = 0; i < m_buttons.size(); ++i)
		{
			(m_hovered == i ? m_buttons[i].hovered : m_buttons[i].normal).draw(rects[i + 1]);
		}
	}

	std::vector<Engine::Rect> ButtonMenu::layout(int width, int height) const
	{
		const float titleWidth = static_cast<float>(width) * m_titleShare;
		const float titleHeight = titleWidth / m_title.aspect();
		const float titleY = static_cast<float>(height) - titleHeight * 3;

		std::vector<Engine::Rect> rects {
			{
				.x = (static_cast<float>(width) - titleWidth) / 2, 
				.y = titleY, 
				.width = titleWidth,
				.height = titleHeight
			} };

		const float scale = titleWidth / static_cast<float>(m_title.width());
		const float gap = titleHeight / 2;
		float y = titleY - gap * 4;

		for (const Button& button : m_buttons)
		{
			const float buttonWidth = static_cast<float>(button.normal.width()) * scale;
			const float buttonHeight = static_cast<float>(button.normal.height()) * scale;

			if (rects.size() > 1)
				y -= buttonHeight + gap;

			rects.push_back({ 
				.x = (static_cast<float>(width) - buttonWidth) / 2, 
				.y = y, 
				.width = buttonWidth, 
				.height = buttonHeight });
		}

		return rects;
	}
}
