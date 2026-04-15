#include "Gamepad.h"
#include "SDL3/SDL.h"

#ifdef WIN32

#include <Windows.h>
#include <Xinput.h>

std::unordered_map<dae::GamepadButton, unsigned int> g_ButtonMap
{
	{dae::GamepadButton::SOUTH,				XINPUT_GAMEPAD_A},
	{dae::GamepadButton::EAST,				XINPUT_GAMEPAD_B},
	{dae::GamepadButton::WEST,				XINPUT_GAMEPAD_X},
	{dae::GamepadButton::NORTH,				XINPUT_GAMEPAD_Y},
	{dae::GamepadButton::BACK,				XINPUT_GAMEPAD_BACK},
	{dae::GamepadButton::START,				XINPUT_GAMEPAD_START},
	{dae::GamepadButton::LEFT_THUMB,		XINPUT_GAMEPAD_LEFT_THUMB},
	{dae::GamepadButton::RIGHT_THUMB,		XINPUT_GAMEPAD_RIGHT_THUMB},
	{dae::GamepadButton::LEFT_SHOULDER,		XINPUT_GAMEPAD_LEFT_SHOULDER},
	{dae::GamepadButton::RIGHT_SHOULDER,	XINPUT_GAMEPAD_RIGHT_SHOULDER},
	{dae::GamepadButton::DPAD_UP,			XINPUT_GAMEPAD_DPAD_UP},
	{dae::GamepadButton::DPAD_DOWN,			XINPUT_GAMEPAD_DPAD_DOWN},
	{dae::GamepadButton::DPAD_LEFT,			XINPUT_GAMEPAD_DPAD_LEFT},
	{dae::GamepadButton::DPAD_RIGHT,		XINPUT_GAMEPAD_DPAD_RIGHT}
};

class dae::Gamepad::Impl
{
public:
	void ProcessInput();

	bool IsDownThisFrame(unsigned int button) const;
	bool IsUpThisFrame(unsigned int button) const;
	bool IsPressed(unsigned int button) const;

private:

	DWORD m_ControllerIndex{};
	WORD m_ButtonsPressedThisFrame{};
	WORD m_ButtonsReleasedThisFrame{};
	XINPUT_STATE m_CurrentState{};
	XINPUT_STATE m_PreviousState{};
};

void dae::Gamepad::Impl::ProcessInput()
{
	// get the first controller you can find
	for (DWORD i = 0; i < 4; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS)
		{
			m_ControllerIndex = i;
			break;
		}
	}

	//XINPUT_GAMPAD__GUID

	CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	XInputGetState(m_ControllerIndex, &m_CurrentState);

	WORD buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
	m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
	m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
}

bool dae::Gamepad::Impl::IsDownThisFrame(unsigned int button) const
{
	return m_ButtonsPressedThisFrame & button;
}

bool dae::Gamepad::Impl::IsUpThisFrame(unsigned int button) const
{
	return m_ButtonsReleasedThisFrame & button;
}

bool dae::Gamepad::Impl::IsPressed(unsigned int button) const
{
	return m_CurrentState.Gamepad.wButtons & button;
}

#else

class dae::Gamepad::Impl
{
public:
	Impl();
	~Impl();

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

dae::Gamepad::Impl::Impl()
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

dae::Gamepad::Impl::~Impl()
{
	m_Controller = nullptr;
}

void dae::Gamepad::Impl::ProcessInput()
{
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
	for (int button{ 0 }; button < static_cast<int>(dae::GamepadButton::COUNT); ++button)
	{
		if (!SDL_GetGamepadButton(m_Controller, static_cast<SDL_GamepadButton>(button))) continue;

		m_CurrentState |= (1u << button);
	}

	uint32_t buttonChanges = m_CurrentState ^ m_PreviousState;

	m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState;
	m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState);
}

bool dae::Gamepad::Impl::IsPressed(unsigned int button) const
{
	return m_CurrentState & (1u << button);
}

bool dae::Gamepad::Impl::IsDownThisFrame(unsigned int button) const
{
	return m_ButtonsPressedThisFrame & (1u << button);
}

bool dae::Gamepad::Impl::IsUpThisFrame(unsigned int button) const
{
	return m_ButtonsReleasedThisFrame & (1u << button);
}

#endif

dae::Gamepad::Gamepad()
{
	m_pImpl = std::make_unique<Impl>();
}

dae::Gamepad::~Gamepad() = default;

void dae::Gamepad::ProcessInput()
{
	m_pImpl->ProcessInput();
}

bool dae::Gamepad::IsDownThisFrame(GamepadButton button) const
{
#ifdef WIN32
	auto itr = g_ButtonMap.find(button);
	if (itr == g_ButtonMap.end()) return false;

	return m_pImpl->IsDownThisFrame(itr->second);
#else
	return m_pImpl->IsDownThisFrame(static_cast<unsigned int>(button));
#endif
}

bool dae::Gamepad::IsUpThisFrame(GamepadButton button) const
{
#ifdef WIN32
	auto itr = g_ButtonMap.find(button);
	if (itr == g_ButtonMap.end()) return false;

	return m_pImpl->IsUpThisFrame(itr->second);
#else
	return m_pImpl->IsUpThisFrame(static_cast<unsigned int>(button));
#endif
}

bool dae::Gamepad::IsPressed(GamepadButton button) const
{
#ifdef WIN32
	auto itr = g_ButtonMap.find(button);
	if (itr == g_ButtonMap.end()) return false;

	return m_pImpl->IsPressed(itr->second);
#else
	return m_pImpl->IsPressed(static_cast<unsigned int>(button));
#endif
}
