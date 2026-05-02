#include "MiniginLoggingSoundSystem.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <queue>

#include <thread>
#include <mutex>

#include <iostream>

// ref: https://github.com/libsdl-org/SDL_mixer/blob/main/examples/basics/01-load-and-play/load-and-play.c

class dae::MiniginLoggingSoundSystem::Impl final
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

void dae::MiniginLoggingSoundSystem::Impl::Play(const std::string& file, const float volume)
{
	if (file.empty() || volume <= 0) return;

	{
		std::scoped_lock<std::mutex> lock{ m_Mutex };

		SoundEvent newSound{};
		newSound.file = file;
		newSound.volume = std::min(volume, 1.f);

		m_SoundQueue.push(newSound);
		std::cout << m_Root << newSound.file << " has been added to the sound queue.\n";
	}

	m_Condition.notify_one();
}

void dae::MiniginLoggingSoundSystem::Impl::Init()
{
	// posible way to fix my all white emscripten
	//SDL_AudioSpec spec = {};
	//spec.freq = 44100;
	//spec.format = SDL_AUDIO_S16;
	//spec.channels = 2;

	m_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

	if (m_Mixer == nullptr)
	{
		throw std::runtime_error(std::string("Couldn't create mixer on default device: ") + SDL_GetError());
	}

	m_PlaySounds = true;
	m_Thread = std::jthread{ &Impl::PlaySoundQueue, this };
}

void dae::MiniginLoggingSoundSystem::Impl::Quit()
{
	{
		std::scoped_lock<std::mutex> lock{ m_Mutex };
		m_PlaySounds = false;
	}

	m_Condition.notify_one();
	MIX_DestroyMixer(m_Mixer);
}

void dae::MiniginLoggingSoundSystem::Impl::PlaySoundQueue()
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

void dae::MiniginLoggingSoundSystem::Impl::PlaySound(const SoundEvent& sound)
{
	std::string path{ m_Root + sound.file };

	MIX_Audio* audio = MIX_LoadAudio(m_Mixer, path.c_str(), false);

	if (!audio)
	{
		std::cout << std::string("Failed to open file: ") + SDL_GetError();
		return;
	}

	MIX_Track* track = MIX_CreateTrack(m_Mixer);

	if (!track)
	{
		std::cout << std::string("Failed to create track: ") + SDL_GetError();
		return;
	}

	std::cout << "Successfully loaded " << path << ", the sound will be played at " << sound.volume * 100 << "% volume.\n";

	MIX_SetTrackGain(track, sound.volume);
	MIX_SetTrackAudio(track, audio);

	MIX_PlayTrack(track, 0);

	// fire and release => alternative to an old MIX_PLAY_AUTOFREE macro
	// https://wiki.libsdl.org/SDL3_mixer/MIX_TrackStoppedCallback
	auto onTrackStopped = [](void*, MIX_Track* track)
		{
			MIX_DestroyTrack(track);
			std::cout << "Sound has been released.\n";
		};

	MIX_SetTrackStoppedCallback(track, onTrackStopped, nullptr);

	// can be released without stopping the track
	MIX_DestroyAudio(audio);
}

dae::MiniginLoggingSoundSystem::MiniginLoggingSoundSystem(const std::string& root) 
	: m_pImpl{ std::make_unique<dae::MiniginLoggingSoundSystem::Impl>(root) } {}

dae::MiniginLoggingSoundSystem::~MiniginLoggingSoundSystem() = default;
void dae::MiniginLoggingSoundSystem::Play(const std::string& file, const float volume) { m_pImpl->Play(file, volume); }
void dae::MiniginLoggingSoundSystem::Init() { m_pImpl->Init(); }
void dae::MiniginLoggingSoundSystem::Quit() { m_pImpl->Quit(); }
