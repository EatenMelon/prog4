#pragma once
#include <SoundSystem.h>
#include <memory>

namespace minigin
{
	class NullSoundSystem final : public ISoundSystem
	{
	public:
		void Play(const std::string&, const float ) override {};

		void Mute(bool) override { };
		bool IsMuted() const override { return false; };

		void Init() override {};
		void Quit() override {};

	};

	class ServiceLocator final
	{
	public:
		static void RegisterSoundSystem(std::unique_ptr<ISoundSystem>&& ss) { m_SoundSystem = std::move(ss); }
		static ISoundSystem* GetSoundSystem() { return m_SoundSystem.get(); }

	private:
		inline static std::unique_ptr<ISoundSystem> m_SoundSystem{ std::make_unique<NullSoundSystem>() };

	};
}
