#include "FpsCounterComponent.h"

#include <iomanip>
#include <sstream>

#include "GameObject.h"
#include "TextComponent.h"

void dae::FpsCounterComponent::Init()
{
	m_TextComponent = GetOwner().GetComponent<TextComponent>();
}

void dae::FpsCounterComponent::Update([[maybe_unused]] float deltaTime)
{
	if (m_TextComponent == nullptr) return;

	constexpr float updateDelay{ 0.75f };
	
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
