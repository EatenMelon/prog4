#include "MiniginLoggingSoundSystem.h"

#include <iostream>

minigin::LoggingSoundSystem::LoggingSoundSystem(std::unique_ptr<ISoundSystem> ss)
	: m_Wrapped{ std::move(ss) }
{}

minigin::LoggingSoundSystem::~LoggingSoundSystem() = default;

void minigin::LoggingSoundSystem::Play(const std::string& file, const float volume)
{
	std::cout << "Playing sound \"" << file << "\" at " << volume * 100 << "% volume!\n";
	m_Wrapped->Play(file, volume);
}

void minigin::LoggingSoundSystem::Init()
{
	m_Wrapped->Init();
	std::cout << "Initialized sound system!\n";
}

void minigin::LoggingSoundSystem::Quit()
{
	std::cout << "Quitting sound system!\n";
	m_Wrapped->Quit();
}
