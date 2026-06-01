#pragma once
#include <string>

namespace minigin
{
	class ISoundSystem
	{
	public:
		virtual ~ISoundSystem() = default;

		virtual void Play(const std::string& file, const float volume) = 0;

		virtual void Init() = 0;
		virtual void Quit() = 0;

	};
}
