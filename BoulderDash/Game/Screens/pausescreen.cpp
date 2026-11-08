#include "pausescreen.h"

namespace BoulderDash::Screens
{
	namespace
	{
		constexpr std::size_t resumeButton = 0, saveButton = 1;
	}

	PauseScreen::PauseScreen(const Resources& resources)
		: m_menu(resources.box, resources.pause, 1 / 4.f, {
			  { resources.resume, resources.resumeHovered },
			  { resources.save, resources.saveHovered },
		  })
	{
	}

	PauseScreen::Choice PauseScreen::update(const Engine::Input& input)
	{
		const std::optional<std::size_t> clicked = m_menu.update(input);
		if (clicked == resumeButton || input.wasPressed(Engine::Key::Escape))
			return Choice::Resume;
		if (clicked == saveButton)
			return Choice::Save;
		return Choice::None;
	}

	void PauseScreen::draw(int width, int height) const
	{
		m_menu.draw(width, height);
	}
}
