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

		static const std::unordered_map<SDL_GamepadButton, unsigned int> m_InputMap;

#ifdef WIN32
		class XinputImpl;
		std::unique_ptr<XinputImpl> m_pImpl{};
#else
		class SDLImpl;
		std::unique_ptr<SDLImpl> m_pImpl1{};
#endif

	};
}
