#include "GamepadXinput.h"

#include <Windows.h>
#include <Xinput.h>

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
	return m_pImpl->IsDownThisFrame(static_cast<unsigned int>(button));
}

bool dae::Gamepad::IsUpThisFrame(GamepadButton button) const
{
	return m_pImpl->IsUpThisFrame(static_cast<unsigned int>(button));
}

bool dae::Gamepad::IsPressed(GamepadButton button) const
{
	return m_pImpl->IsPressed(static_cast<unsigned int>(button));
}
