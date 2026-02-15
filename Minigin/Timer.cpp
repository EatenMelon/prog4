#include "Timer.h"
#include <thread>

void dae::Timer::SetFPS(int fps)
{
	m_FixedFrameTime = 1.f / fps;

}

void dae::Timer::Start()
{
	m_LastTime = std::chrono::high_resolution_clock::now();
}

void dae::Timer::Stop()
{
	m_CurrentTime = std::chrono::high_resolution_clock::now();
	m_DeltaTime = std::chrono::duration<float>(m_CurrentTime - m_LastTime).count();
}

void dae::Timer::WaitUntilNextFrame() const
{
	const auto sleepTime{ m_CurrentTime + std::chrono::milliseconds(int(1000 * m_FixedFrameTime)) - std::chrono::high_resolution_clock::now() };
	std::this_thread::sleep_for(sleepTime);
}

float dae::Timer::GetDeltaTime() const
{
	return m_DeltaTime;
}

float dae::Timer::GetFixedFrameTime() const
{
	return m_FixedFrameTime;
}
