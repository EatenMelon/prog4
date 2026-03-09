#include "FpsCounterComponent.h"

#include <iomanip>
#include <sstream>

#include "GameObject.h"
#include "TextComponent.h"

void dae::FpsCounterComponent::Init()
{
	m_TextComponent = GetOwner().GetComponent<TextComponent>();
}

void dae::FpsCounterComponent::Update(float deltaTime)
{
	m_Controller.ProcessInput();

	if (m_Controller.IsPressed(SDL_GAMEPAD_BUTTON_DPAD_RIGHT))
	{
		auto pos = GetOwner().GetLocalPosition();

		pos.x += 100.f * deltaTime;

		GetOwner().SetLocalPosition(pos);
	}

	if (m_TextComponent == nullptr) return;

	constexpr float updateDelay{ 1.f };
	
	if (m_UntilTextUpdate > 0.f)
	{
		m_UntilTextUpdate -= deltaTime;
		return;
	}

	m_UntilTextUpdate = updateDelay;

	std::stringstream ss{};
	ss << std::fixed << std::setprecision(1) << 1.f / deltaTime << " FPS";

	m_TextComponent->SetText(ss.str());
}
