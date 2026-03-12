#include "GamepadSDL.h"
#include <unordered_map>
#include <cstdint>

class dae::Gamepad::SDLImpl
{
public:
	SDLImpl();
	~SDLImpl();

	void ProcessInput();

	bool IsDownThisFrame(unsigned int button) const;
	bool IsUpThisFrame(unsigned int button) const;
	bool IsPressed(unsigned int button) const;

private:
	SDL_Gamepad* m_Controller{};

	uint32_t m_ButtonsPressedThisFrame{};
	uint32_t m_ButtonsReleasedThisFrame{};
	uint32_t m_CurrentState{};
	uint32_t m_PreviousState{};
};

dae::Gamepad::SDLImpl::SDLImpl()
{
	// get the fist controller that is currently connected
	int num_joysticks{ 0 };
	SDL_JoystickID* joysticks = SDL_GetJoysticks(&num_joysticks);

	if (!joysticks || num_joysticks <= 0) return;

	// check if the first joystick is a supported gamepad
	if (SDL_IsGamepad(joysticks[0]))
	{
		m_Controller = SDL_OpenGamepad(joysticks[0]);
	}

	// free the array returned by SDL_GetJoysticks
	SDL_free(joysticks);
}

dae::Gamepad::SDLImpl::~SDLImpl()
{
	m_Controller = nullptr;
}

void dae::Gamepad::SDLImpl::ProcessInput()
{
	if (!m_Controller) return;

	// need this function for the emscripten build
	SDL_PumpEvents();

	SDL_UpdateGamepads();

	m_PreviousState = m_CurrentState;
	m_CurrentState = 0;

	// seach for a controller when you currently don't have one
	if (!m_Controller)
	{
		int num_joysticks{ 0 };
		SDL_JoystickID* joysticks = SDL_GetJoysticks(&num_joysticks);

		if (!joysticks || num_joysticks <= 0) return;

		if (SDL_IsGamepad(joysticks[0]))
		{
			m_Controller = SDL_OpenGamepad(joysticks[0]);
		}

		SDL_free(joysticks);
	}

	// trying a similar approach as XInput
	// but XInput uses bitmasks while SDL uses enums
	// so I'll need to do a bitshift instead of simply doing AND
	for (int button{ 0 }; button < SDL_GAMEPAD_BUTTON_COUNT; ++button)
	{
		if (!SDL_GetGamepadButton(m_Controller, static_cast<SDL_GamepadButton>(button))) continue;

		m_CurrentState |= (1 << button);
	}

	uint32_t buttonChanges = m_CurrentState ^ m_PreviousState;

	m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState;
	m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState);
}

bool dae::Gamepad::SDLImpl::IsPressed(unsigned int button) const
{
	return m_CurrentState & (1 << button);
}

bool dae::Gamepad::SDLImpl::IsDownThisFrame(unsigned int button) const
{
	return m_ButtonsPressedThisFrame & (1 << button);
}

bool dae::Gamepad::SDLImpl::IsUpThisFrame(unsigned int button) const
{
	return m_ButtonsReleasedThisFrame & (1 << button);
}

dae::Gamepad::Gamepad()
{
	m_pImpl = std::make_unique<SDLImpl>();
}

dae::Gamepad::~Gamepad() = default;

void dae::Gamepad::ProcessInput()
{
	m_pImpl->ProcessInput();
}

bool dae::Gamepad::IsDownThisFrame(GamepadButton button) const
{
	return m_pImpl->IsDownThisFrame(button);
}

bool dae::Gamepad::IsUpThisFrame(GamepadButton button) const
{
	return m_pImpl->IsUpThisFrame(button);
}

bool dae::Gamepad::IsPressed(GamepadButton button) const
{
	return m_pImpl->IsPressed(button);
}
