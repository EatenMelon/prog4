#include "MiniginLoggingSoundSystem.h"

#include <iostream>

dae::MiniginLoggingSoundSystem::MiniginLoggingSoundSystem(std::unique_ptr<SoundSystem> ss)
	: m_Wrapped{ std::move(ss) }
{}

dae::MiniginLoggingSoundSystem::~MiniginLoggingSoundSystem() = default;

void dae::MiniginLoggingSoundSystem::Play(const std::string& file, const float volume)
{
	std::cout << "Playing sound \"" << file << "\" at " << volume * 100 << "% volume!\n";
	m_Wrapped->Play(file, volume);
}

void dae::MiniginLoggingSoundSystem::Init()
{
	m_Wrapped->Init();
	std::cout << "Initialized sound system!\n";
}

void dae::MiniginLoggingSoundSystem::Quit()
{
	std::cout << "Quitting sound system!\n";
	m_Wrapped->Quit();
}
