#pragma once

#include "resources.h"
#include "../Engine/input.h"

#include <memory>

namespace BoulderDash
{
	// What the game shows: the menu, a level...
	class Screen
	{
	public:
		explicit Screen(const Resources& resources) : m_resources(resources) { }
		virtual ~Screen() = default;

		// Advances the screen by `time` ms; returns the screen to show next, or null to stay.
		virtual std::unique_ptr<Screen> update(int time, const Engine::Input& input) = 0;
		virtual void draw(int width, int height) const = 0;

	protected:
		const Resources& resources() const { return m_resources; }

	private:
		const Resources& m_resources;
	};
}
