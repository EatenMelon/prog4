#include <Controller.h>
#include <unordered_map>
#include <SDL3/SDL_oldnames.h>

#pragma region Xinput

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
	for (DWORD i = 0; i < 4; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS)
		{
			// Controller i is connected
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

#pragma endregion

#pragma region SDL

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
	int m_Index{};

	std::unordered_map<unsigned int, bool> m_CurrentState{};
	std::unordered_map<unsigned int, bool> m_PreviousState{};
};

dae::Gamepad::SDLImpl::SDLImpl()
{

}

dae::Gamepad::SDLImpl::~SDLImpl()
{
	m_Controller = nullptr;
}

void dae::Gamepad::SDLImpl::ProcessInput()
{
	if (!m_Controller) return;

	// Copy previous button states
	m_PreviousState = m_CurrentState;

	// Update buttons
	for (int b = SDL_GAMEPAD_BUTTON_SOUTH; b < SDL_GAMEPAD_BUTTON_COUNT; ++b)
	{
		bool pressed = SDL_GetGamepadButton(m_Controller, static_cast<SDL_GamepadButton>(b)) != 0;
		m_CurrentState[static_cast<SDL_GamepadButton>(b)] = pressed;
	}
}

bool dae::Gamepad::SDLImpl::IsDownThisFrame(unsigned int button) const
{
	bool curr = m_CurrentState.count(button) ? m_CurrentState.at(button) : false;
	bool prev = m_PreviousState.count(button) ? m_PreviousState.at(button) : false;
	return curr && !prev;
}

bool dae::Gamepad::SDLImpl::IsUpThisFrame(unsigned int button) const
{
	bool curr = m_CurrentState.count(button) ? m_CurrentState.at(button) : false;
	bool prev = m_PreviousState.count(button) ? m_PreviousState.at(button) : false;
	return !curr && prev;
}

bool dae::Gamepad::SDLImpl::IsPressed(unsigned int button) const
{
	auto it = m_CurrentState.find(button);
	return it != m_CurrentState.end() && it->second;
}

#pragma endregion

dae::Gamepad::Gamepad()
	: m_pImpl{ std::make_unique<XinputImpl>() }
	, m_pImpl1{ std::make_unique<SDLImpl>() }
{
}

dae::Gamepad::~Gamepad() = default;

void dae::Gamepad::ProcessInput()
{
	m_pImpl1->ProcessInput();
}

bool dae::Gamepad::IsDownThisFrame(unsigned int button) const
{
	return m_pImpl1->IsDownThisFrame(button);
}

bool dae::Gamepad::IsUpThisFrame(unsigned int button) const
{
	return m_pImpl1->IsUpThisFrame(button);
}

bool dae::Gamepad::IsPressed(unsigned int button) const
{
	return m_pImpl1->IsPressed(button);
}
