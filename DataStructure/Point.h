#pragma once
namespace ds
{
	struct Point
	{
		double x, y;
		constexpr Point& operator+=(const Point &rhs) { return x += rhs.x, y += rhs.y, *this; }
		constexpr Point& operator-=(const Point &rhs) { return x -= rhs.x, y -= rhs.y, *this; }
		friend constexpr Point operator+(Point lhs, const Point &rhs) { return lhs += rhs; }
		friend constexpr Point operator-(Point lhs, const Point &rhs) { return lhs -= rhs; }

	};
	constexpr Point operator""_x(unsigned long long x) { return { (double)x,0 }; }
	constexpr Point operator""_y(unsigned long long y) { return { 0,(double)y }; }
	constexpr Point operator""_x(long double x) { return { (double)x,0 }; }
	constexpr Point operator""_y(long double y) { return { 0,(double)y }; }

}