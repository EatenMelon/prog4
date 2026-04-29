#pragma once

#include <SoundSystem.h>
#include <memory>

namespace dae
{
	class MiniginSoundSystem final : public SoundSystem
	{
	public:
		MiniginSoundSystem(const std::string& root = "");
		~MiniginSoundSystem() override;

		void Play(const std::string& file, const float volume) override;

		void Init() override;
		void Quit() override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl{};
	};
}