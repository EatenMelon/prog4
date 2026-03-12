#pragma once
#include <unordered_map>
#include <memory>

#include "SDL3/SDL.h"

namespace dae
{
	class Gamepad
	{
	public:
		Gamepad();
		~Gamepad();
		void ProcessInput();

		bool IsDownThisFrame(SDL_GamepadButton button) const;
		bool IsUpThisFrame(SDL_GamepadButton button) const;
		bool IsPressed(SDL_GamepadButton button) const;

	private:
		unsigned int GetButton(SDL_GamepadButton button) const;

#ifdef _WIN32
		static const std::unordered_map<SDL_GamepadButton, unsigned int> m_InputMap;

		class Impl;
		std::unique_ptr<Impl> m_pImpl{};
#else
		class SDLImpl;
		std::unique_ptr<SDLImpl> m_pImpl{};
#endif

	};
}
