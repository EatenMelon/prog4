#pragma once
#include <memory>
#include <glm/glm.hpp>

#ifdef WIN32
#include <wtypes.h>
#endif

namespace minigin
{
	enum class GamepadButton
	{
		SOUTH = 0,
		EAST = 1,
		WEST = 2,
		NORTH = 3,
		BACK = 4,
		START = 6,
		LEFT_THUMB = 7,
		RIGHT_THUMB = 8,
		LEFT_SHOULDER = 9,
		RIGHT_SHOULDER = 10,
		DPAD_UP = 11,
		DPAD_DOWN = 12,
		DPAD_LEFT = 13,
		DPAD_RIGHT = 14,
		COUNT = 15
	};

	enum class GamepadJoystick
	{
		LEFT_JOYSTICK,
		RIGHT_JOYSTICK
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

		glm::vec2 GetJoystick(GamepadJoystick joystick) const;
		void SetDeadzone(float deadzone);

	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl{};
	};
}