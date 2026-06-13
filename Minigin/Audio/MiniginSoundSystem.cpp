#include "MiniginSoundSystem.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <queue>

#include <thread>
#include <mutex>
#include <unordered_map>

// ref: https://github.com/libsdl-org/SDL_mixer/blob/main/examples/basics/01-load-and-play/load-and-play.c
class minigin::SoundSystem::Impl final
{
public:
	Impl(const std::string& root) : m_Root{ root } {}
	void Play(const std::string& file, const float volume);

	void Mute(bool mute);
	bool IsMuted() const { return m_Mute; }

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
	void LoadAndPlaySound(const SoundEvent& sound);

	const std::string m_Root{""};

	std::thread m_Thread{};
	std::mutex m_Mutex{};
	std::condition_variable m_Condition{};
	bool m_PlaySounds{ false };
	bool m_Mute{ false };

	std::queue<SoundEvent> m_SoundQueue{};
	MIX_Mixer* m_Mixer{ nullptr };

	std::unordered_map<std::string, MIX_Track*> m_LoadedSounds{};
};

void minigin::SoundSystem::Impl::Play(const std::string& file, const float volume)
{
	if (file.empty() || volume <= 0)
	{
		return;
	}

	{
		std::lock_guard<std::mutex> lock{ m_Mutex };

		SoundEvent newSound{};
		newSound.file = file;
		newSound.volume = std::min(volume, 1.f);

		m_SoundQueue.push(newSound);
	}

	m_Condition.notify_one();
}

void minigin::SoundSystem::Impl::Mute(bool mute)
{
	m_Mute = mute;

	if (m_Mute)
	{
		MIX_StopAllTracks(m_Mixer, 0);
	}
}

void minigin::SoundSystem::Impl::Init()
{
	m_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

	if (m_Mixer == nullptr)
	{
		throw std::runtime_error(std::string("Couldn't create mixer on default device: ") + SDL_GetError());
	}

	m_PlaySounds = true;
	m_Thread = std::thread{ &Impl::PlaySoundQueue, this };
}

void minigin::SoundSystem::Impl::Quit()
{
	{
		std::lock_guard<std::mutex> lock{ m_Mutex };
		m_PlaySounds = false;
	}
	m_Condition.notify_one();

	if (m_Thread.joinable())
	{
		m_Thread.join();
	}

	for (auto [path, track] : m_LoadedSounds)
	{
		MIX_DestroyTrack(track);
	}

	MIX_StopAllTracks(m_Mixer, 0);
	MIX_DestroyMixer(m_Mixer);
	m_Mixer = nullptr;
}

void minigin::SoundSystem::Impl::PlaySoundQueue()
{
	while (m_PlaySounds)
	{
		std::unique_lock<std::mutex> lock{ m_Mutex };

		m_Condition.wait(lock, [&]() { return !m_SoundQueue.empty() || !m_PlaySounds; });

		if (!m_PlaySounds) break;

		auto next = m_SoundQueue.front();
		m_SoundQueue.pop();
		
		lock.unlock();
		PlaySound(next);
	}
}

void minigin::SoundSystem::Impl::PlaySound(const SoundEvent& sound)
{
	if (m_Mute) return;

	std::string path{ m_Root + sound.file };

	auto loadedSound = m_LoadedSounds.find(path);

	if (loadedSound == m_LoadedSounds.end())
	{
		LoadAndPlaySound(sound);
		return;
	}

	auto track = loadedSound->second;

	if (MIX_TrackPlaying(track)) return;

	MIX_PlayTrack(track, 0);
}

void minigin::SoundSystem::Impl::LoadAndPlaySound(const SoundEvent& sound)
{
	std::string path{ m_Root + sound.file };

	MIX_Audio* audio = MIX_LoadAudio(m_Mixer, path.c_str(), false);

	if (!audio)
	{
		throw std::runtime_error(std::string("Failed to open file: ") + std::string(SDL_GetError()));
	}

	MIX_Track* track = MIX_CreateTrack(m_Mixer);

	if (!track)
	{
		throw std::runtime_error(std::string("Failed to create track: ") + SDL_GetError());
	}

	MIX_SetTrackGain(track, sound.volume);
	MIX_SetTrackAudio(track, audio);

	m_LoadedSounds.emplace(path, track);
	MIX_PlayTrack(track, 0);

	// can be released without stopping the track
	MIX_DestroyAudio(audio);
}

minigin::SoundSystem::SoundSystem(const std::string& root) : m_pImpl{ std::make_unique<minigin::SoundSystem::Impl>(root) } {}
minigin::SoundSystem::~SoundSystem() = default;
void minigin::SoundSystem::Play(const std::string& file, const float volume) { m_pImpl->Play(file, volume); }
void minigin::SoundSystem::Mute(bool mute) { m_pImpl->Mute(mute); }
bool minigin::SoundSystem::IsMuted() const { return m_pImpl->IsMuted(); }
void minigin::SoundSystem::Init() { m_pImpl->Init(); }
void minigin::SoundSystem::Quit() { m_pImpl->Quit(); }
