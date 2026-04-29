#pragma once
#include <SoundSystem.h>
#include <memory>

namespace dae
{
	class NullSoundSystem final : public SoundSystem
	{
	public:
		void Play(const std::string&, const float ) override {};

		void Init() override {};
		void Quit() override {};

	};

	class ServiceLocator final
	{
	public:
		static void RegisterSoundSystem(std::unique_ptr<SoundSystem>&& ss) { m_SoundSystem = std::move(ss); }
		static SoundSystem* GetSoundSystem() { return m_SoundSystem.get(); }

	private:
		inline static std::unique_ptr<SoundSystem> m_SoundSystem{ std::make_unique<NullSoundSystem>() };

	};
}
