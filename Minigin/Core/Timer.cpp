#include "Timer.h"
#include <thread>

void minigin::Timer::SetFPS(int fps)
{
	m_FixedFrameTime = 1.f / fps;

}

void minigin::Timer::Start()
{
	m_LastTime = std::chrono::high_resolution_clock::now();
}

void minigin::Timer::Stop()
{
	m_CurrentTime = std::chrono::high_resolution_clock::now();
	m_DeltaTime = std::chrono::duration<float>(m_CurrentTime - m_LastTime).count();
}

void minigin::Timer::WaitUntilNextFrame() const
{
	const auto sleepTime{ m_CurrentTime + std::chrono::milliseconds(int(1000 * m_FixedFrameTime)) - std::chrono::high_resolution_clock::now() };
	std::this_thread::sleep_for(sleepTime);
}

float minigin::Timer::GetDeltaTime() const
{
	return m_DeltaTime;
}

float minigin::Timer::GetFixedFrameTime() const
{
	return m_FixedFrameTime;
}
