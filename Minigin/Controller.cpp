#include <Controller.h>
#include <unordered_map>
#include <cstdint>

#ifdef WIN32

#include <Windows.h>
#include <Xinput.h>

class dae::Gamepad::XinputImpl
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

void dae::Gamepad::XinputImpl::ProcessInput()
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

	CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	XInputGetState(m_ControllerIndex, &m_CurrentState);

	WORD buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
	m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
	m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
}

bool dae::Gamepad::XinputImpl::IsDownThisFrame(unsigned int button) const
{
	return m_ButtonsPressedThisFrame & button;
}

bool dae::Gamepad::XinputImpl::IsUpThisFrame(unsigned int button) const
{
	return m_ButtonsReleasedThisFrame & button;
}

bool dae::Gamepad::XinputImpl::IsPressed(unsigned int button) const
{
	return m_CurrentState.Gamepad.wButtons & button;
}

#else

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

	uint16_t m_ButtonsPressedThisFrame{};
	uint16_t m_ButtonsReleasedThisFrame{};
	uint16_t m_CurrentState{};
	uint16_t m_PreviousState{};
};

dae::Gamepad::SDLImpl::SDLImpl()
{
	// get the fist controller that is currently connected
	int num_joysticks = 0;
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

	SDL_UpdateGamepads();

	m_PreviousState = m_CurrentState;
	m_CurrentState = 0;

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
#endif

#ifdef WIN32
const std::unordered_map<SDL_GamepadButton, uint32_t> dae::Gamepad::m_InputMap
{
	{ SDL_GAMEPAD_BUTTON_SOUTH,				XINPUT_GAMEPAD_A },
	{ SDL_GAMEPAD_BUTTON_EAST,				XINPUT_GAMEPAD_B },
	{ SDL_GAMEPAD_BUTTON_WEST,				XINPUT_GAMEPAD_X },
	{ SDL_GAMEPAD_BUTTON_NORTH,				XINPUT_GAMEPAD_Y },

	{ SDL_GAMEPAD_BUTTON_DPAD_UP,			XINPUT_GAMEPAD_DPAD_UP },
	{ SDL_GAMEPAD_BUTTON_DPAD_DOWN,			XINPUT_GAMEPAD_DPAD_DOWN },
	{ SDL_GAMEPAD_BUTTON_DPAD_LEFT,			XINPUT_GAMEPAD_DPAD_LEFT },
	{ SDL_GAMEPAD_BUTTON_DPAD_RIGHT,		XINPUT_GAMEPAD_DPAD_RIGHT },

	{ SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,		XINPUT_GAMEPAD_LEFT_SHOULDER },
	{ SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,	XINPUT_GAMEPAD_RIGHT_SHOULDER },
};
#endif

dae::Gamepad::Gamepad()
{
#ifdef WIN32
	m_pImpl = std::make_unique<XinputImpl>();
#else
	m_pImpl = std::make_unique<SDLImpl>();
#endif
}

dae::Gamepad::~Gamepad() = default;

void dae::Gamepad::ProcessInput()
{
	m_pImpl->ProcessInput();
}

bool dae::Gamepad::IsDownThisFrame(SDL_GamepadButton button) const
{
	return m_pImpl->IsDownThisFrame(GetButton(button));
}

bool dae::Gamepad::IsUpThisFrame(SDL_GamepadButton button) const
{
	return m_pImpl->IsUpThisFrame(GetButton(button));
}

bool dae::Gamepad::IsPressed(SDL_GamepadButton button) const
{
	return m_pImpl->IsPressed(GetButton(button));
}

unsigned int dae::Gamepad::GetButton(SDL_GamepadButton button) const
{
#ifdef WIN32

	auto it = m_InputMap.find(button);

	if (it != m_InputMap.end())
	{
		return it->second;
	}

#endif

	return static_cast<unsigned int>(button);
}
