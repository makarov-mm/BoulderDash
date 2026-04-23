#ifndef POINT_H
#define POINT_H

namespace Engine
{
	template<typename T> struct Point
	{
		T x, y;
		Point(T x, T y): x(x), y(y) { }
		Point(): x(0), y(0) { }
	};
}

#endif
