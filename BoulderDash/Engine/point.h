#pragma once

namespace Engine
{
	template<typename T>
	struct Point
	{
		T x{}, y{};

		constexpr Point operator+(Point other) const { return { x + other.x, y + other.y }; }
		constexpr Point operator-(Point other) const { return { x - other.x, y - other.y }; }
		constexpr bool operator==(const Point&) const = default;
	};

	// A rectangle on the screen; y grows upwards.
	struct Rect
	{
		float x = 0, y = 0, width = 0, height = 0;

		constexpr bool contains(Point<float> point) const
		{
			return point.x >= x && point.x <= x + width && point.y >= y && point.y <= y + height;
		}
	};
}
