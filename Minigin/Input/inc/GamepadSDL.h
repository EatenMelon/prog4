#pragma once
#include <unordered_map>
#include <memory>

#include "SDL3/SDL.h"

namespace dae
{
	enum class GamepadButton
	{
		DPAD_UP			= SDL_GAMEPAD_BUTTON_DPAD_UP,
		DPAD_DOWN		= SDL_GAMEPAD_BUTTON_DPAD_DOWN,
		DPAD_LEFT		= SDL_GAMEPAD_BUTTON_DPAD_LEFT,
		DPAD_RIGHT		= SDL_GAMEPAD_BUTTON_DPAD_RIGHT,

		START			= SDL_GAMEPAD_BUTTON_START,
		BACK			= SDL_GAMEPAD_BUTTON_BACK,

		LEFT_THUMB		= SDL_GAMEPAD_BUTTON_LEFT_STICK,
		RIGHT_THUMB		= SDL_GAMEPAD_BUTTON_RIGHT_STICK,
		LEFT_SHOULDER	= SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
		RIGHT_SHOULDER	= SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,

		EAST			= SDL_GAMEPAD_BUTTON_EAST,
		SOUTH			= SDL_GAMEPAD_BUTTON_SOUTH,
		WEST			= SDL_GAMEPAD_BUTTON_WEST,
		NORTH			= SDL_GAMEPAD_BUTTON_NORTH,

		COUNT
	};

	class Gamepad
	{
	public:
		Gamepad();
		~Gamepad();
		void ProcessInput();

		bool IsDownThisFrame(GamepadButton button) const;
		bool IsUpThisFrame(GamepadButton button) const;
		bool IsPressed(GamepadButton button) const;

	private:
		class SDLImpl;
		std::unique_ptr<SDLImpl> m_pImpl{};

	};
}