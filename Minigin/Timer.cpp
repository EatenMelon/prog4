#include "Timer.h"
#include <thread>

void Timer::SetFPS(int fps)
{
	m_FixedFrameTime = 1.f / fps;

}

void Timer::Start()
{
	m_LastTime = std::chrono::high_resolution_clock::now();
}

void Timer::Stop()
{
	m_CurrentTime = std::chrono::high_resolution_clock::now();
	m_Duration = std::chrono::duration<float>(m_CurrentTime - m_LastTime).count();
}

void Timer::WaitUntilNextFrame() const
{
	const auto sleepTime{ m_CurrentTime + std::chrono::milliseconds(int(1000 * m_FixedFrameTime)) - std::chrono::high_resolution_clock::now() };
	std::this_thread::sleep_for(sleepTime);
}

float Timer::GetDuration() const
{
	return m_Duration;
}

float Timer::GetFixedFrameTime() const
{
	return m_FixedFrameTime;
}
