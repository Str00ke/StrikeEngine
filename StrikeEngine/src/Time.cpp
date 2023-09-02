#include "Time.hpp"
#include <iostream>

namespace StrikeEngine
{

	void Time::Init()
	{
		m_startTime = std::chrono::high_resolution_clock::now();
	}

	bool Time::Update()
	{
		auto currTime = std::chrono::high_resolution_clock::now();
		m_currTime = std::chrono::duration<float, std::chrono::seconds::period>(currTime - m_startTime).count();
		m_deltaTime = m_currTime - m_prevTime;
		m_prevTime = m_currTime;
		return true;
	}

	std::chrono::steady_clock::time_point Time::GetStartTime()
	{
		return m_startTime;
	}

	float Time::GetCurrTime()
	{
		return m_currTime;
	}

	const float Time::DeltaTime()
	{
		return m_deltaTime;
	}

}
