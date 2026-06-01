#pragma once

#include <SoundSystem.h>
#include <memory>

namespace minigin
{
	class LoggingSoundSystem final : public ISoundSystem
	{
	public:
		LoggingSoundSystem(std::unique_ptr<ISoundSystem> ss);
		~LoggingSoundSystem() override;

		void Play(const std::string& file, const float volume) override;

		void Init() override;
		void Quit() override;

	private:
		std::unique_ptr<ISoundSystem> m_Wrapped{};
	};
}