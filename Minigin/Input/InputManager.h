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
		void Init(int gamepads);

		bool ProcessInput(float deltaTime);

		void BindInput(const std::string& name, unsigned int button, KeyState state,
			std::shared_ptr<Command> command, int playerID, Direction axisDirection = Direction::None);

		void BindInput(const std::string& name, GamepadButton button, KeyState state,
			std::shared_ptr<Command> command, int playerID, Direction axisDirection = Direction::None);

		void BindInput(const std::string& name, GamepadJoystick joystick, float deadzone, std::shared_ptr<Command> command, int playerID);

		void UnBindInput(const std::string& actionName);
		void Enable(bool enable) { m_IsEnabled = enable; }

		static int GetGamepadLimit() { return m_MaxGamepads; }
		static int GetKeyboardID() { return m_KeyboardPlayerID; }

	private:
		static const int m_MaxGamepads;
		static const int m_KeyboardPlayerID;

		bool ValidatePlayerId(int playerID, bool canBeKeyboard) const;

		bool m_IsEnabled{ true };

		std::vector<std::unique_ptr<Gamepad>> m_Gamepads{};

		struct ButtonBinding
		{
			std::string actionName{};
			unsigned int button{};
			KeyState state{};
			std::shared_ptr<Command> command{};
			Direction axisDirection{};
			int playerID{ -1 };
		};

		struct JoystickBinding
		{
			std::string actionName{};
			GamepadJoystick joystick{};
			float deadzone{};
			std::shared_ptr<Command> command{};
			int playerID{ -1 };
		};

		struct PlayerButton
		{
			unsigned int button{};
			int playerID{};

			bool operator==(const PlayerButton& other) const noexcept
			{
				return button == other.button &&
					playerID == other.playerID;
			}
		};

		struct PlayerButtonHash
		{
			// hash function for unordered maps
			// Used the example of the PointHasher from geeksforgeeks:
			// https://www.geeksforgeeks.org/cpp/implement-custom-hash-functions-for-user-defined-types-in-std-unordered_map/
			std::size_t operator()(const PlayerButton& pb) const noexcept
			{
				std::size_t h1 = std::hash<unsigned int>{}(pb.button);
				std::size_t h2 = std::hash<int>{}(pb.playerID);

				return h1 ^ (h2 << 1);
			}
		};

		std::vector<ButtonBinding> m_ButtonBindings{};
		std::vector<JoystickBinding> m_JoystickBindings{};
		std::unordered_map<PlayerButton, KeyState, PlayerButtonHash> m_KeysDown{ };
	};
}
