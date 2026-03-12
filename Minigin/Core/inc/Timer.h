#pragma once
#include <chrono>

namespace dae
{
	class Timer
	{
	public:
		Timer() = default;
		~Timer() = default;

		Timer(const Timer& other) = delete;
		Timer& operator=(const Timer& other) = delete;
		Timer(Timer&& other) = delete;
		Timer& operator=(Timer&& other) = delete;

		void SetFPS(int fps);

		void Start();
		void Stop();

		void WaitUntilNextFrame() const;

		float GetDeltaTime() const;
		float GetFixedFrameTime() const;

	private:

		float m_FixedFrameTime{};
		float m_DeltaTime{};

		std::chrono::high_resolution_clock::time_point m_LastTime{};
		std::chrono::high_resolution_clock::time_point m_CurrentTime{};

	};
}
