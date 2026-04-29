#include "MiniginSoundSystem.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <queue>

#include <thread>
#include <mutex>

// ref: https://github.com/libsdl-org/SDL_mixer/blob/main/examples/basics/01-load-and-play/load-and-play.c

class dae::MiniginSoundSystem::Impl final
{
public:
	Impl(const std::string& root) : m_Root{ root } {}
	void Play(const std::string& file, const float volume);

	void Init();
	void Quit();

private:
	struct SoundEvent
	{
		std::string file{};
		float volume{};
	};

	void PlaySoundQueue();
	void PlaySound(const SoundEvent& sound);

	const std::string m_Root{""};

	std::jthread m_Thread{};
	std::mutex m_Mutex{};
	std::condition_variable m_Condition{};
	bool m_PlaySounds{ false };

	std::queue<SoundEvent> m_SoundQueue{};
	MIX_Mixer* m_Mixer{ nullptr };
};

void dae::MiniginSoundSystem::Impl::Play(const std::string& file, const float volume)
{
	if (file.empty() || volume <= 0) return;

	{
		std::scoped_lock<std::mutex> lock{ m_Mutex };

		SoundEvent newSound{};
		newSound.file = file;
		newSound.volume = std::max(volume, 1.f);

		m_SoundQueue.push(newSound);
	}

	m_Condition.notify_one();
}

void dae::MiniginSoundSystem::Impl::Init()
{
	MIX_Init();

	m_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

	if (m_Mixer == nullptr)
	{
		throw std::runtime_error(std::string("Couldn't create mixer on default device: ") + SDL_GetError());
	}

	m_PlaySounds = true;
	m_Thread = std::jthread{ &Impl::PlaySoundQueue, this };
}

void dae::MiniginSoundSystem::Impl::Quit()
{
	{
		std::scoped_lock<std::mutex> lock{ m_Mutex };
		m_PlaySounds = false;
	}
	m_Condition.notify_one();

	MIX_DestroyMixer(m_Mixer);
	MIX_Quit();
}

void dae::MiniginSoundSystem::Impl::PlaySoundQueue()
{
	while (m_PlaySounds)
	{
		std::unique_lock<std::mutex> lock{ m_Mutex };

		m_Condition.wait(lock, [&]() { return !m_SoundQueue.empty() || !m_PlaySounds; });

		if (!m_PlaySounds) break;

		auto next = m_SoundQueue.front();
		m_SoundQueue.pop();
		
		PlaySound(next);
	}
}

void dae::MiniginSoundSystem::Impl::PlaySound(const SoundEvent& sound)
{
	std::string path{ m_Root + sound.file };

	MIX_Audio* audio = MIX_LoadAudio(m_Mixer, path.c_str(), false);
	MIX_Track* track = MIX_CreateTrack(m_Mixer);
	MIX_SetTrackGain(track, sound.volume);
	MIX_SetTrackAudio(track, audio);

	MIX_PlayTrack(track, 0);

	// fire and release => alternative to an old MIX_PLAY_AUTOFREE macro
	// https://wiki.libsdl.org/SDL3_mixer/MIX_TrackStoppedCallback
	auto onTrackStopped = [](void*, MIX_Track* track)
		{
			MIX_DestroyTrack(track);
		};

	MIX_SetTrackStoppedCallback(track, onTrackStopped, nullptr);

	// can be released without stopping the track
	MIX_DestroyAudio(audio);
}

dae::MiniginSoundSystem::MiniginSoundSystem(const std::string& root) : m_pImpl{ std::make_unique<dae::MiniginSoundSystem::Impl>(root) } {}
dae::MiniginSoundSystem::~MiniginSoundSystem() = default;
void dae::MiniginSoundSystem::Play(const std::string& file, const float volume) { m_pImpl->Play(file, volume); }
void dae::MiniginSoundSystem::Init() { m_pImpl->Init(); }
void dae::MiniginSoundSystem::Quit() { m_pImpl->Quit(); }
