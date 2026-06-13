#pragma once

#include <SoundSystem.h>
#include <memory>

namespace minigin
{
	class SoundSystem final : public ISoundSystem
	{
	public:
		SoundSystem(const std::string& root = "");
		~SoundSystem() override;

		void Play(const std::string& file, const float volume) override;

		void Mute(bool mute) override;
		bool IsMuted() const override;

		void Init() override;
		void Quit() override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl{};
	};
}