#pragma once

#include <Windows.h>
#include <Xinput.h>
#include "SDL3/SDL.h"
#include <memory>
#include <SDL3/SDL_gamepad.h>
#include <unordered_map>

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

#ifdef WIN32
		class XinputImpl;
		std::unique_ptr<XinputImpl> m_pImpl{};
#else
		class SDLImpl;
		std::unique_ptr<SDLImpl> m_pImpl1{};
#endif

	};
}
