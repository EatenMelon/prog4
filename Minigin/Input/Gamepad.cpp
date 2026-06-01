#include "Gamepad.h"
#include "SDL3/SDL.h"
#include <algorithm>

//#undef WIN32
#ifdef WIN32

#include <Windows.h>
#include <Xinput.h>
#include <unordered_map>

std::unordered_map<minigin::GamepadButton, unsigned int> g_ButtonMap
{
	{minigin::GamepadButton::SOUTH,				XINPUT_GAMEPAD_A},
	{minigin::GamepadButton::EAST,				XINPUT_GAMEPAD_B},
	{minigin::GamepadButton::WEST,				XINPUT_GAMEPAD_X},
	{minigin::GamepadButton::NORTH,				XINPUT_GAMEPAD_Y},
	{minigin::GamepadButton::BACK,				XINPUT_GAMEPAD_BACK},
	{minigin::GamepadButton::START,				XINPUT_GAMEPAD_START},
	{minigin::GamepadButton::LEFT_THUMB,		XINPUT_GAMEPAD_LEFT_THUMB},
	{minigin::GamepadButton::RIGHT_THUMB,		XINPUT_GAMEPAD_RIGHT_THUMB},
	{minigin::GamepadButton::LEFT_SHOULDER,		XINPUT_GAMEPAD_LEFT_SHOULDER},
	{minigin::GamepadButton::RIGHT_SHOULDER,	XINPUT_GAMEPAD_RIGHT_SHOULDER},
	{minigin::GamepadButton::DPAD_UP,			XINPUT_GAMEPAD_DPAD_UP},
	{minigin::GamepadButton::DPAD_DOWN,			XINPUT_GAMEPAD_DPAD_DOWN},
	{minigin::GamepadButton::DPAD_LEFT,			XINPUT_GAMEPAD_DPAD_LEFT},
	{minigin::GamepadButton::DPAD_RIGHT,		XINPUT_GAMEPAD_DPAD_RIGHT}
};

class minigin::Gamepad::Impl
{
public:
	Impl(int playerID);

	void ProcessInput();

	bool IsDownThisFrame(unsigned int button) const;
	bool IsUpThisFrame(unsigned int button) const;
	bool IsPressed(unsigned int button) const;

	glm::vec2 GetJoystick(GamepadJoystick joystick) const;
	void SetDeadzone(float deadzone) { m_Deadzone = deadzone; }

private:
	glm::vec2 ApplyDeadzone(const glm::vec2& in) const;

	DWORD m_ControllerIndex{};
	WORD m_ButtonsPressedThisFrame{};
	WORD m_ButtonsReleasedThisFrame{};
	XINPUT_STATE m_CurrentState{};
	XINPUT_STATE m_PreviousState{};

	glm::vec2 m_LeftJoystick{ glm::vec2() };
	glm::vec2 m_RightJoystick{ glm::vec2() };

	float m_Deadzone{ 0.05f };
	int m_PlayerID{ -1 };
};

minigin::Gamepad::Impl::Impl(int playerID) 
	: m_ControllerIndex{ static_cast<DWORD>(std::clamp(playerID, 0, 3)) }
{
}

void minigin::Gamepad::Impl::ProcessInput()
{
	CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));

	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	if (XInputGetState(m_ControllerIndex, &state) == ERROR_SUCCESS)
	{
		m_CurrentState = state;
	}
	else
	{
		ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));

		m_ButtonsPressedThisFrame = 0;
		m_ButtonsReleasedThisFrame = 0;

		m_LeftJoystick = glm::vec2(0.f);
		m_RightJoystick = glm::vec2(0.f);

		return;
	}

	WORD buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
	m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
	m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);

	// update joysticks
	m_LeftJoystick = glm::vec2(m_CurrentState.Gamepad.sThumbLX, -m_CurrentState.Gamepad.sThumbLY);
	m_RightJoystick = glm::vec2(m_CurrentState.Gamepad.sThumbRX, -m_CurrentState.Gamepad.sThumbRY);

	//std::cout << "Left stick: [" << m_LeftJoystick.x << ", " << m_LeftJoystick.y << "] \t";
	//std::cout << "Right stick: [" << m_RightJoystick.x << ", " << m_RightJoystick.y << "]\n";
}

bool minigin::Gamepad::Impl::IsDownThisFrame(unsigned int button) const
{
	return m_ButtonsPressedThisFrame & button;
}

bool minigin::Gamepad::Impl::IsUpThisFrame(unsigned int button) const
{
	return m_ButtonsReleasedThisFrame & button;
}

bool minigin::Gamepad::Impl::IsPressed(unsigned int button) const
{
	return m_CurrentState.Gamepad.wButtons & button;
}

glm::vec2 minigin::Gamepad::Impl::GetJoystick(GamepadJoystick joystick) const
{
	switch (joystick)
	{
	case minigin::GamepadJoystick::LEFT_JOYSTICK:
		return ApplyDeadzone(m_LeftJoystick);
		break;

	case minigin::GamepadJoystick::RIGHT_JOYSTICK:
		return ApplyDeadzone(m_RightJoystick);
		break;
	}

	return glm::vec2();
}

glm::vec2 minigin::Gamepad::Impl::ApplyDeadzone(const glm::vec2& in) const
{
	glm::vec2 v{ in };

	// normalize
	constexpr float xinputMax = 32767.f;

	v /= xinputMax;

	float length2 = glm::dot(v, v);

	if (length2 < m_Deadzone * m_Deadzone)
	{
		v = glm::vec2(0.f);
		return v;
	}

	float length = glm::sqrt(length2);

	glm::vec2 dir = v / length;

	// remap safely
	float t = (length - m_Deadzone) / (1.0f - m_Deadzone);
	t = glm::clamp(t, 0.0f, 1.0f);

	v = dir * t;

	return v;
}

#else

class minigin::Gamepad::Impl
{
public:
	Impl(int playerID);
	~Impl();

	void ProcessInput();

	bool IsDownThisFrame(unsigned int button) const;
	bool IsUpThisFrame(unsigned int button) const;
	bool IsPressed(unsigned int button) const;

	glm::vec2 GetJoystick(GamepadJoystick joystick) const;
	void SetDeadzone(float deadzone) { m_Deadzone = deadzone; }

private:
	void GetController();
	glm::vec2 ApplyDeadzone(const glm::vec2& in) const;

	SDL_Gamepad* m_Controller{};

	uint32_t m_ButtonsPressedThisFrame{};
	uint32_t m_ButtonsReleasedThisFrame{};
	uint32_t m_CurrentState{};
	uint32_t m_PreviousState{};

	glm::vec2 m_LeftJoystick{ glm::vec2() };
	glm::vec2 m_RightJoystick{ glm::vec2() };

	float m_Deadzone{ 0.05f };
	int m_PlayerID{ -1 };
};

minigin::Gamepad::Impl::Impl(int playerID) : m_PlayerID{ playerID }
{
	m_PlayerID = std::clamp(m_PlayerID, 0, 3);

	GetController();
}

minigin::Gamepad::Impl::~Impl()
{
	SDL_CloseGamepad(m_Controller);
	m_Controller = nullptr;
}

void minigin::Gamepad::Impl::ProcessInput()
{
	// need this function for the emscripten build
	SDL_PumpEvents();

	SDL_UpdateGamepads();

	m_PreviousState = m_CurrentState;
	m_CurrentState = 0;

	// seach for a controller when you currently don't have one
	if (!m_Controller)
	{
		GetController();
	}

	// trying a similar approach as XInput
	// but XInput uses bitmasks while SDL uses enums
	// so I'll need to do a bitshift instead of simply doing AND
	for (int button{ 0 }; button < static_cast<int>(minigin::GamepadButton::COUNT); ++button)
	{
		if (!SDL_GetGamepadButton(m_Controller, static_cast<SDL_GamepadButton>(button))) continue;

		m_CurrentState |= (1u << button);
	}

	uint32_t buttonChanges = m_CurrentState ^ m_PreviousState;

	m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState;
	m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState);

	// updating joysticks

	m_LeftJoystick.x = SDL_GetGamepadAxis(m_Controller, SDL_GAMEPAD_AXIS_LEFTX);
	m_LeftJoystick.y = SDL_GetGamepadAxis(m_Controller, SDL_GAMEPAD_AXIS_LEFTY);

	m_RightJoystick.x = SDL_GetGamepadAxis(m_Controller, SDL_GAMEPAD_AXIS_RIGHTX);
	m_RightJoystick.y = SDL_GetGamepadAxis(m_Controller, SDL_GAMEPAD_AXIS_RIGHTY);

	//std::cout << "Left stick: [" << m_LeftJoystick.x << ", " << m_LeftJoystick.y << "] \t";
	//std::cout << "Right stick: [" << m_RightJoystick.x << ", " << m_RightJoystick.y << "]\n";
}

bool minigin::Gamepad::Impl::IsPressed(unsigned int button) const
{
	return m_CurrentState & (1u << button);
}

bool minigin::Gamepad::Impl::IsDownThisFrame(unsigned int button) const
{
	return m_ButtonsPressedThisFrame & (1u << button);
}

bool minigin::Gamepad::Impl::IsUpThisFrame(unsigned int button) const
{
	return m_ButtonsReleasedThisFrame & (1u << button);
}

glm::vec2 minigin::Gamepad::Impl::GetJoystick(GamepadJoystick joystick) const
{
	switch (joystick)
	{
	case minigin::GamepadJoystick::LEFT_JOYSTICK:
		return ApplyDeadzone(m_LeftJoystick);
		break;

	case minigin::GamepadJoystick::RIGHT_JOYSTICK:
		return ApplyDeadzone(m_RightJoystick);
		break;
	}

	return glm::vec2();
}

void minigin::Gamepad::Impl::GetController()
{
	// not the best way since this doesn't consider disconnecting hardware
	int numJoysticks{ 0 };
	SDL_JoystickID* joysticks = SDL_GetJoysticks(&numJoysticks);

	if (!joysticks || numJoysticks <= 0) return;
	if (numJoysticks <= m_PlayerID) return;

	if (SDL_IsGamepad(joysticks[m_PlayerID]))
	{
		m_Controller = SDL_OpenGamepad(joysticks[m_PlayerID]);
	}

	SDL_free(joysticks);
}

glm::vec2 minigin::Gamepad::Impl::ApplyDeadzone(const glm::vec2& in) const
{
	glm::vec2 v{ in };

	// normalize
	constexpr float xinputMax = 32767.f;

	v /= xinputMax;

	float length2 = glm::dot(v, v);

	if (length2 < m_Deadzone * m_Deadzone)
	{
		v = glm::vec2(0.f);
		return v;
	}

	float length = glm::sqrt(length2);

	glm::vec2 dir = v / length;

	// remap safely
	float t = (length - m_Deadzone) / (1.0f - m_Deadzone);
	t = glm::clamp(t, 0.0f, 1.0f);

	v = dir * t;

	return v;
}

#endif

minigin::Gamepad::Gamepad(int playerID)
{
	m_pImpl = std::make_unique<Impl>(playerID);
}

minigin::Gamepad::~Gamepad() = default;

void minigin::Gamepad::ProcessInput()
{
	m_pImpl->ProcessInput();
}

bool minigin::Gamepad::IsDownThisFrame(GamepadButton button) const
{
#ifdef WIN32
	auto itr = g_ButtonMap.find(button);
	if (itr == g_ButtonMap.end()) return false;

	return m_pImpl->IsDownThisFrame(itr->second);
#else
	return m_pImpl->IsDownThisFrame(static_cast<unsigned int>(button));
#endif
}

bool minigin::Gamepad::IsUpThisFrame(GamepadButton button) const
{
#ifdef WIN32
	auto itr = g_ButtonMap.find(button);
	if (itr == g_ButtonMap.end()) return false;

	return m_pImpl->IsUpThisFrame(itr->second);
#else
	return m_pImpl->IsUpThisFrame(static_cast<unsigned int>(button));
#endif
}

bool minigin::Gamepad::IsPressed(GamepadButton button) const
{
#ifdef WIN32
	auto itr = g_ButtonMap.find(button);
	if (itr == g_ButtonMap.end()) return false;

	return m_pImpl->IsPressed(itr->second);
#else
	return m_pImpl->IsPressed(static_cast<unsigned int>(button));
#endif
}

glm::vec2 minigin::Gamepad::GetJoystick(GamepadJoystick joystick) const
{
	return m_pImpl->GetJoystick(joystick);
}

void minigin::Gamepad::SetDeadzone(float deadzone)
{
	return m_pImpl->SetDeadzone(deadzone);
}
