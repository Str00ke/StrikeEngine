#pragma once
#include <chrono>


namespace StrikeEngine
{
	static class Time
	{
	public:
		static void Init();
		static bool Update();

		static std::chrono::steady_clock::time_point GetStartTime();
		static float GetCurrTime();
	private:
		inline static std::chrono::steady_clock::time_point m_startTime = {};
		inline static float m_currTime = 0.0f;
	};

}

