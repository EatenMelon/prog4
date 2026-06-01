#include "HealthDisplay.h"

#include <sstream>

#include "TextComponent.h"
#include "HealthComponent.h"
#include "GameObject.h"

void digdug::HealthDisplay::Init()
{
	UpdateDisplay();
}

void digdug::HealthDisplay::OnNotify(minigin::IEvent event)
{
	if (event != minigin::IEvent::ReceivedDamage) return;

	UpdateDisplay();
}

void digdug::HealthDisplay::UpdateDisplay()
{
	if (m_Display == nullptr)
	{
		auto txtComp = GetOwner().GetComponent<minigin::TextComponent>();

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
