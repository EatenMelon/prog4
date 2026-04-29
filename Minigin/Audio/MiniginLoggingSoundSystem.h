#pragma once

#include <SoundSystem.h>
#include <memory>

namespace dae
{
	class MiniginLoggingSoundSystem final : public SoundSystem
	{
	public:
		MiniginLoggingSoundSystem(const std::string& root = "");
		~MiniginLoggingSoundSystem() override;

		void Play(const std::string& file, const float volume) override;

		void Init() override;
		void Quit() override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl{};
	};
}