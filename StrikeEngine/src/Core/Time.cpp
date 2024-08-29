#include "Core/Time.hpp"
#include <iostream>

namespace StrikeEngine
{
#ifdef VK_API

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

	



#else

Time::Time() :
	m_secondesPerCount(0.0), m_deltaTime(-1.0), m_baseTime(0),
	m_pausedTime(0), m_prevTime(0), m_currTime(0), m_stopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondesPerCount = 1.0 / (double)countsPerSec;
}

float Time::gameTime() const
{
	return 0.0f;
}

float Time::totalTime() const
{
	// If we are stopped, do not count the time that has passed
	// since we stopped. Moreover, if we previously already had
	// a pause, the distance mStopTime - mBaseTime includes paused
	// time,which we do not want to count. To correct this, we can
	// subtract the paused time from mStopTime:
	//
	// previous paused time
	// |<----------->|
	// ---*------------*-------------*-------*-----------*------> time
	// m_baseTime				m_stopTime m_currTime

	if (m_stopped)
		return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondesPerCount);

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count. To correct this, we can subtract
	// the paused time from mCurrTime:
	//
	// (mCurrTime - mPausedTime) - mBaseTime
	//
	// |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	// m_baseTime	m_stopTime startTime m_currTime
	else
		return (float)(((m_currTime - m_pausedTime) - m_baseTime) * m_secondesPerCount);
}

float Time::deltaTime() const
{
	return (float)m_deltaTime;
}

void Time::reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_baseTime = currTime;
	m_prevTime = currTime;
	m_stopTime = 0;
	m_stopped = false;
}

void Time::start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// Accumulate the time elapsed between stop and start pairs.
	//
	// |<-------d------->|	
	// ---------------*-----------------*------------> time
	//			m_stopTime			startTime
	// If we are resuming the timer from a stopped state...
	if (m_stopped)
	{
		// then accumulate the paused time
		m_pausedTime += (startTime - m_stopTime);

		// since we are starting the timer back up, the current
		// previous time is not valid, as it occurred while paused.
		// So reset it to the current time
		m_prevTime = startTime;

		// no longer stopped
		m_stopTime = 0;
		m_stopped = false;
	}
}

void Time::stop()
{
	// If we are already stopped, then don't do anything
	if (!m_stopped)
	{
		__int64 currTime;
		
		// Otherwise, save the time we stopped at, and set the bool flag indicating the timer is stopped
		m_stopTime = currTime;
		m_stopped = true;
	}
}

void Time::tick()
{
	if (m_stopped) 
	{
		m_deltaTime = 0.0;
		return;
	}

	// Get the time for this frame
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	// Time difference between this frame and the previous
	m_deltaTime = (m_currTime - m_prevTime) * m_secondesPerCount;

	// Prepare for next frame
	m_prevTime = m_currTime;

	// Force nonnegative. The DXSDK’s CDXUTTimer mentions that if the
	// processor goes into a power save mode or we get shuffled to
	// another processor, then m_deltaTime can be negative
	if (m_deltaTime < 0.0) m_deltaTime = 0.0;
}

#endif // VK_API

}
