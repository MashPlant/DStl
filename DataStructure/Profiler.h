#pragma once
#include <chrono>
namespace ds
{
	class Profiler
	{
		typedef std::chrono::high_resolution_clock Timer;
		typedef decltype(Timer::now()) ClockType;
		ClockType beg;
	public:
		Profiler() :beg(Timer::now()) {}
		auto reset() ->decltype(Timer::now()- Timer::now())
		{
			auto ret = Timer::now() - beg;
			beg = Timer::now();
			return ret;
		}
	};
}