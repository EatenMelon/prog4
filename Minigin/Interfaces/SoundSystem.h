#pragma once
#include <string>

namespace minigin
{
	class ISoundSystem
	{
	public:
		virtual ~ISoundSystem() = default;

		virtual void Play(const std::string& file, const float volume) = 0;

		virtual void Mute(bool mute) = 0;
		virtual bool IsMuted() const = 0;

		virtual void Init() = 0;
		virtual void Quit() = 0;

	};
}
