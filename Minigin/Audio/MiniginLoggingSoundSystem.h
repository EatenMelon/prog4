#pragma once

#include <SoundSystem.h>
#include <memory>

namespace dae
{
	class MiniginLoggingSoundSystem final : public SoundSystem
	{
	public:
		MiniginLoggingSoundSystem(std::unique_ptr<SoundSystem> ss);
		~MiniginLoggingSoundSystem() override;

		void Play(const std::string& file, const float volume) override;

		void Init() override;
		void Quit() override;

	private:
		std::unique_ptr<SoundSystem> m_Wrapped{};
	};
}