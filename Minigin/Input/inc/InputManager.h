#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Command.h"
#include "GamepadSDL.h"

#include "Singleton.h"
#include <unordered_map>

namespace dae
{
	enum class KeyState { OnDown, OnRelease, Pressed, Idle };
	enum class Direction { Up, Down, Left, Right, None };

	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();

		void BindInput(const std::string& name, unsigned int button, KeyState state, 
			std::shared_ptr<Command> command, Direction axisDirection = Direction::None);

		void BindInput(const std::string& name, GamepadButton button, KeyState state,
			std::shared_ptr<Command> command, Direction axisDirection = Direction::None);

		void UnBindInput(const std::string& actionName);

	private:
		Gamepad m_Gamepad{};

		struct InputBinding
		{
			std::string actionName{};
			unsigned int button{};
			KeyState state{};
			std::shared_ptr<Command> command{};
			Direction axisDirection{};
		};

		std::vector<InputBinding> m_InputBindings{};
		std::unordered_map<unsigned int, KeyState> m_KeysDown{ };
	};
}
