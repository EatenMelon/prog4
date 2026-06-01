#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Command.h"
#include "Gamepad.h"

#include "Singleton.h"
#include <unordered_map>

namespace minigin
{
	enum class KeyState { OnDown, OnRelease, Pressed, Idle };
	enum class Direction { Up, Down, Left, Right, None };

	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput(float deltaTime);

		void BindInput(const std::string& name, unsigned int button, KeyState state, 
			std::shared_ptr<Command> command, Direction axisDirection = Direction::None);

		void BindInput(const std::string& name, GamepadButton button, KeyState state,
			std::shared_ptr<Command> command, Direction axisDirection = Direction::None);

		void BindInput(const std::string& name, GamepadJoystick joystick, float deadzone, std::shared_ptr<Command> command);

		void UnBindInput(const std::string& actionName);

	private:
		Gamepad m_Gamepad{};

		struct ButtonBinding
		{
			std::string actionName{};
			unsigned int button{};
			KeyState state{};
			std::shared_ptr<Command> command{};
			Direction axisDirection{};
		};

		struct JoystickBinding
		{
			std::string actionName{};
			GamepadJoystick joystick{};
			float deadzone{};
			std::shared_ptr<Command> command{};
		};

		std::vector<ButtonBinding> m_ButtonBindings{};
		std::vector<JoystickBinding> m_JoystickBindings{};
		std::unordered_map<unsigned int, KeyState> m_KeysDown{ };
	};
}
