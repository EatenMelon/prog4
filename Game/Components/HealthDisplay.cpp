#include "HealthDisplay.h"

#include <sstream>

#include "TextComponent.h"
#include "HealthComponent.h"
#include "GameObject.h"

void digdug::HealthDisplay::Init()
{
	UpdateDisplay();
}

void digdug::HealthDisplay::OnNotify(const minigin::IEvent* event)
{
	auto plain = static_cast<const minigin::PlainEvent*>(event);

	if (plain->GetEventHash() != minigin::PlainEvent::CreateHashSDBM("damage")) return;

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
