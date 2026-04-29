#include "HealthDisplay.h"

#include <sstream>

#include "TextComponent.h"
#include "HealthComponent.h"
#include "GameObject.h"

void dae::HealthDisplay::Init()
{
	UpdateDisplay();
}

void dae::HealthDisplay::OnNotify(Event event)
{
	if (event != Event::ReceivedDamage) return;

	UpdateDisplay();
}

void dae::HealthDisplay::UpdateDisplay()
{
	if (m_Display == nullptr)
	{
		auto txtComp = GetOwner().GetComponent<TextComponent>();

		if (txtComp == nullptr) return;

		m_Display = txtComp;
	}

	if (m_Health == nullptr)
	{
		auto scoreComp = GetOwner().GetComponent<HealthComponent>();

		if (scoreComp == nullptr) return;

		m_Health = scoreComp;
	}

	std::stringstream ss{};
	ss << "Health: " << m_Health->GetHealth();

	m_Display->SetText(ss.str());
}
