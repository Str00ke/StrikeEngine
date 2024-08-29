#pragma once
#ifdef VK_API
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
		static const float DeltaTime();
	private:
		inline static std::chrono::steady_clock::time_point m_startTime; //TODO: inline static only compiles on >= c++17
		inline static float m_currTime;
		inline static float m_prevTime;
		inline static float m_deltaTime;
	};
}

#else

#include <windows.h>

namespace StrikeEngine
{
	static class Time
	{
	public:
		Time();
		~Time() = default;

		float gameTime() const; // in seconds
		float deltaTime() const; // in seconds
		float totalTime() const; // in seconds
		
		void reset(); // call before message loop
		void start(); // call when unpaused
		void stop(); // call when paused
		void tick(); // call every frame

	private:
		double m_secondesPerCount;
		double m_deltaTime;

		__int64 m_baseTime;
		__int64 m_pausedTime;
		__int64 m_stopTime;
		__int64 m_prevTime;
		__int64 m_currTime;

		bool m_stopped;
	};
}

#endif // VK_API

