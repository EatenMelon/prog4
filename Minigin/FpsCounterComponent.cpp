#include "FpsCounterComponent.h"

#include <string>
#include <iomanip>
#include <sstream>

#include "GameObject.h"
#include "TextObject.h"

void dae::FpsCounterComponent::Update([[maybe_unused]] float deltaTime)
{
	constexpr float updateDelay{ 0.5f };

	if (typeid(GetOwner()) != typeid(TextObject)) return;
	
	if (m_UntilTextUpdate > 0.f)
	{
		m_UntilTextUpdate -= deltaTime;
		return;
	}

	std::stringstream ss{};
	ss << std::fixed << std::setprecision(1) << 1.f / deltaTime << " FPS";

	m_UntilTextUpdate = updateDelay;

	if (m_UpdateTimes <= 0)
	{
		static_cast<TextObject&>(GetOwner()).SetText("destroyed");
		Destroy();
	}
	else
	{
		static_cast<TextObject&>(GetOwner()).SetText(ss.str());
	}

	--m_UpdateTimes;
}
