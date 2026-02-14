#pragma once
#include <string>
#include <functional>
#include <filesystem>

#include "Timer.h"

namespace dae
{
	class Minigin final
	{
		bool m_quit{};

		// game loop variables
		Timer m_Timer{};
		float m_Lag{ 0.f };

	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void()>& load);
		void RunOneFrame();

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;

	};
}