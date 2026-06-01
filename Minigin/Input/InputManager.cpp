#include "InputManager.h"
#include <SDL3/SDL.h>

#include <backends/imgui_impl_sdl3.h>
#include <iostream>

bool minigin::InputManager::ProcessInput(float deltaTime)
{
	// process keyboard input
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		auto& state = m_KeysDown[e.key.key];
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
			m_KeysDown[e.key.key] = minigin::KeyState::OnRelease;
			break;
		}

		// imgui input handling
		ImGui_ImplSDL3_ProcessEvent(&e);
	}

	// handle controller events
	m_Gamepad.ProcessInput();

	for (int button{ 0 }; button < static_cast<int>(minigin::GamepadButton::COUNT); ++button)
	{
		auto gamepadButton = static_cast<GamepadButton>(button);

		if (m_Gamepad.IsDownThisFrame(gamepadButton))
		{
			m_KeysDown[button] = minigin::KeyState::OnDown;
		}
		else if (m_Gamepad.IsUpThisFrame(gamepadButton))
		{
			m_KeysDown[button] = minigin::KeyState::OnRelease;
		}
		else if(m_Gamepad.IsPressed(gamepadButton))
		{
			m_KeysDown[button] = minigin::KeyState::Pressed;
		}
	}

	// prepare the input context for each command
	std::unordered_map<Command*, InputContext> commandContexts{};

	for (auto& binding : m_ButtonBindings)
	{
		auto it = m_KeysDown.find(binding.button);
		if (it == m_KeysDown.end()) continue;

		// only prepare context when the desired key state matches
		if (binding.state != m_KeysDown[binding.button]) continue;

		InputContext& context = commandContexts[binding.command.get()];

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
	}

	for (auto binding : m_JoystickBindings)
	{
		InputContext context{};

		m_Gamepad.SetDeadzone(binding.deadzone);
		context.axis = m_Gamepad.GetJoystick(binding.joystick);

		if (glm::dot(context.axis, context.axis) <= 0.f) continue;

//#undef WIN32

#ifdef WIN32
		// SDL does this already, XInput doesn't
		context.axis.y = -context.axis.y;
#endif

		commandContexts[binding.command.get()] = context;
	}

	

	// execute all necessary commands
	for (auto& [command, context] : commandContexts)
	{
		command->Execute(context, deltaTime);
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
	Direction axisDirection
)

{
	m_ButtonBindings.push_back(ButtonBinding{ name, button, state, command, axisDirection });
}

void minigin::InputManager::BindInput
(
	const std::string& name,
	minigin::GamepadButton button,
	minigin::KeyState state,
	std::shared_ptr<minigin::Command> command,
	Direction axisDirection
)

{
	m_ButtonBindings.push_back(ButtonBinding{ name, static_cast<unsigned int>(button), state, command, axisDirection });
}

void minigin::InputManager::BindInput(const std::string& name, GamepadJoystick joystick, float deadzone, std::shared_ptr<Command> command)
{
	m_JoystickBindings.push_back(JoystickBinding{ name, joystick, deadzone, command });
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
