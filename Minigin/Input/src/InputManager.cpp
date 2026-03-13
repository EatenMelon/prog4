#include <SDL3/SDL.h>
#include "InputManager.h"

#include <backends/imgui_impl_sdl3.h>

bool dae::InputManager::ProcessInput()
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
			if (state != dae::KeyState::Pressed && state != dae::KeyState::OnDown)
			{
				state = dae::KeyState::OnDown;
			}
			break;

		case SDL_EVENT_KEY_UP:
			m_KeysDown[e.key.key] = dae::KeyState::OnRelease;
			break;
		}

		// imgui input handling
		ImGui_ImplSDL3_ProcessEvent(&e);
	}

	// handle controller events
	m_Gamepad.ProcessInput();

#ifdef WIN32
	for(DWORD button = 1; button <= 0x8000; button <<= 1)
#else
	for (int button{ 0 }; button < static_cast<int>(dae::GamepadButton::COUNT); ++button)
#endif
	{
		auto gamepadButton = static_cast<GamepadButton>(button);

		if (m_Gamepad.IsDownThisFrame(gamepadButton))
		{
			m_KeysDown[button] = dae::KeyState::OnDown;
		}
		else if (m_Gamepad.IsUpThisFrame(gamepadButton))
		{
			m_KeysDown[button] = dae::KeyState::OnRelease;
		}
		else if(m_Gamepad.IsPressed(gamepadButton))
		{
			m_KeysDown[button] = dae::KeyState::Pressed;
		}
	}

	// prepare the input context for each command
	std::unordered_map<Command*, InputContext> commandContexts{};

	for (auto& binding : m_InputBindings)
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

		if (glm::length(context.axis) > 0.f)
		{
			context.axis = glm::normalize(context.axis);
		}
		else
		{
			// prevent division by zero
			context.axis = { 0.f, 0.f };
		}
	}

	// execute all necessary commands
	for (auto& [command, context] : commandContexts)
	{
		command->Execute(context);
	}

	// make sure on down & on release are single time events
	// kind of reverse engineering SDL (T^T)
	for (auto& [key, state] : m_KeysDown)
	{
		if (state == dae::KeyState::OnDown) state = dae::KeyState::Pressed;
		if (state == dae::KeyState::OnRelease) state = dae::KeyState::Idle;
	}

	return true;
}

void dae::InputManager::BindInput
(
	const std::string& name,
	unsigned int button,
	dae::KeyState state,
	std::shared_ptr<dae::Command> command,
	Direction axisDirection
)

{
	m_InputBindings.push_back(InputBinding{ name, button, state, command, axisDirection });
}

void dae::InputManager::BindInput
(
	const std::string& name,
	dae::GamepadButton button,
	dae::KeyState state,
	std::shared_ptr<dae::Command> command,
	Direction axisDirection
)

{
	m_InputBindings.push_back(InputBinding{ name, static_cast<unsigned int>(button), state, command, axisDirection });
}

void dae::InputManager::UnBindInput(const std::string& actionName)
{
	// remove all bindings with the same name as the given name
	m_InputBindings.erase
	(
		std::remove_if
		(
			m_InputBindings.begin(),
			m_InputBindings.end(),

			[&actionName](const InputBinding& b)
			{
				return b.actionName == actionName;
			}
		),
		m_InputBindings.end()
	);
}
