#include "InputManager.h"
#include <SDL3/SDL.h>

#include <backends/imgui_impl_sdl3.h>
#include <iostream>
#include <algorithm>

void minigin::InputManager::Init(int numPlayers)
{
	m_Gamepads.clear();
	m_Gamepads.reserve(numPlayers);

	if (numPlayers > 4)
	{
		throw std::runtime_error("MAx limit of players is 4");
	}

	for (int p{ 0 }; p < numPlayers; ++p)
	{
		m_Gamepads.push_back(std::make_unique<Gamepad>(p));
	}
}

bool minigin::InputManager::ProcessInput(float deltaTime)
{
	// process keyboard input
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		PlayerButton pb{};
		pb.button = e.key.key;
		pb.playerID = -1;	// keyboard

		auto& state = m_KeysDown[pb];
		switch (e.type)
		{
		case SDL_EVENT_QUIT:
			return false;
			break;

		case SDL_EVENT_KEY_DOWN:
			if (state != minigin::KeyState::Pressed && state != minigin::KeyState::OnDown)
			{
				state = minigin::KeyState::OnDown;
			}
			break;

		case SDL_EVENT_KEY_UP:
			state = minigin::KeyState::OnRelease;
			break;
		}

		// imgui input handling
		ImGui_ImplSDL3_ProcessEvent(&e);
	}

	// handle controller events
	// prepare the input context for each command
	std::vector<std::pair<Command*, InputContext>> commandContexts;

	for (int playerID{ 0 }; playerID < static_cast<int>(m_Gamepads.size()); ++playerID)
	{
		auto& gamepad = m_Gamepads[playerID];
		gamepad->ProcessInput();

		// preparing gamepad button input
		for (int button{ 0 }; button < static_cast<int>(minigin::GamepadButton::COUNT); ++button)
		{
			auto gamepadButton = static_cast<GamepadButton>(button);

			PlayerButton pb{};
			pb.button = button;
			pb.playerID = playerID;

			if (gamepad->IsDownThisFrame(gamepadButton))
			{
				m_KeysDown[pb] = minigin::KeyState::OnDown;
			}
			else if (gamepad->IsUpThisFrame(gamepadButton))
			{
				m_KeysDown[pb] = minigin::KeyState::OnRelease;
			}
			else if (gamepad->IsPressed(gamepadButton))
			{
				m_KeysDown[pb] = minigin::KeyState::Pressed;
			}
		}

		for (const auto& binding : m_JoystickBindings)
		{
			if (binding.playerID != playerID) continue;

			InputContext context{};

			gamepad->SetDeadzone(binding.deadzone);

			context.axis = gamepad->GetJoystick(binding.joystick);
			context.playerID = binding.playerID;

			if (glm::dot(context.axis, context.axis) < 0.0001f) continue;

			commandContexts.emplace_back(binding.command.get(), context);
		}
	}

	// processing all button input
	for (const auto& binding : m_ButtonBindings)
	{
		PlayerButton pb{};
		pb.button = binding.button;
		pb.playerID = binding.playerID;

		auto it = m_KeysDown.find(pb);
		if (it == m_KeysDown.end()) continue;

		// only prepare context when the desired key state matches
		if (binding.state != m_KeysDown[pb]) continue;

		InputContext context{};
		context.playerID = binding.playerID;

		// already changed the axis' y pos to match the screen
		switch (binding.axisDirection)
		{
		case Direction::Up:    context.axis.y -= 1.f; break;
		case Direction::Down:  context.axis.y += 1.f; break;
		case Direction::Left:  context.axis.x -= 1.f; break;
		case Direction::Right: context.axis.x += 1.f; break;
		case Direction::None:  break;
		}

		if (glm::dot(context.axis, context.axis) > 0.f)
		{
			context.axis = glm::normalize(context.axis);
		}
		else
		{
			// prevent division by zero
			context.axis = { 0.f, 0.f };
		}

		commandContexts.emplace_back(binding.command.get(), context);
	}

	// execute all necessary commands
	for (auto& [command, context] : commandContexts)
	{
		command->Execute(context, deltaTime);
		//std::cout << "Executing command for player " << context.playerID << "\n";
	}

	// make sure on down & on release are single time events
	// kind of reverse engineering SDL (T^T)
	for (auto& [key, state] : m_KeysDown)
	{
		if (state == minigin::KeyState::OnDown) state = minigin::KeyState::Pressed;
		if (state == minigin::KeyState::OnRelease) state = minigin::KeyState::Idle;
	}

	return true;
}

void minigin::InputManager::BindInput
(
	const std::string& name,
	unsigned int button,
	minigin::KeyState state,
	std::shared_ptr<minigin::Command> command,
	int playerID,
	Direction axisDirection
)

{
	ButtonBinding binding{};
	binding.actionName = name;
	binding.button = button;
	binding.state = state;
	binding.command = command;
	binding.axisDirection = axisDirection;
	binding.playerID = playerID;

	m_ButtonBindings.push_back(binding);
}

void minigin::InputManager::BindInput
(
	const std::string& name,
	minigin::GamepadButton button,
	minigin::KeyState state,
	std::shared_ptr<minigin::Command> command,
	int playerID,
	Direction axisDirection
)

{
	ButtonBinding binding{};
	binding.actionName = name;
	binding.button = static_cast<unsigned int>(button);
	binding.state = state;
	binding.command = command;
	binding.axisDirection = axisDirection;
	binding.playerID = playerID;

	m_ButtonBindings.push_back(binding);
}

void minigin::InputManager::BindInput
(
	const std::string& name,
	GamepadJoystick joystick,
	float deadzone,
	std::shared_ptr<Command> command,
	int playerID
)
{
	JoystickBinding binding{};
	binding.actionName = name;
	binding.joystick = joystick;
	binding.deadzone = deadzone;
	binding.command = command;
	binding.playerID = playerID;

	m_JoystickBindings.push_back(binding);
}

void minigin::InputManager::UnBindInput(const std::string& actionName)
{
	// remove all bindings with the same name as the given name
	m_ButtonBindings.erase
	(
		std::remove_if
		(
			m_ButtonBindings.begin(),
			m_ButtonBindings.end(),

			[&actionName](const ButtonBinding& b)
			{
				return b.actionName == actionName;
			}
		),
		m_ButtonBindings.end()
	);

	m_JoystickBindings.erase
	(
		std::remove_if
		(
			m_JoystickBindings.begin(),
			m_JoystickBindings.end(),

			[&actionName](const JoystickBinding& j)
			{
				return j.actionName == actionName;
			}
		),
		m_JoystickBindings.end()
	);
}