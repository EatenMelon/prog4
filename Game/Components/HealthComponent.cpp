#include "HealthComponent.h"

#include <sstream>
#include <iostream>

#include <TextComponent.h>
#include <GameObject.h>
#include <Events.h>
#include <Hitbox.h>

#include <KillingComponent.h>

void digdug::HealthComponent::Start()
{
	minigin::HitEvent event{ nullptr };
	m_HitEventHash = minigin::PlainEvent::CreateHashSDBM(event.GetEventName());

	auto hitbox = GetOwner().GetComponent<minigin::Hitbox>();
	if (hitbox != nullptr)
	{
		hitbox->HitEnterEvent().Subscribe(this);
	}

	m_Health = m_MaxHealth;
	UpdateDisplay();
}

void digdug::HealthComponent::OnNotify(const minigin::IEvent& event)
{
	if (event.GetEventHash() != m_HitEventHash) return;

	auto hitEvent = static_cast<const minigin::HitEvent*>(&event);

	auto killComp = hitEvent->Who()->GetOwner().GetComponent<digdug::KillingComponent>();
	if (killComp == nullptr) return;
	if (!killComp->IsTarget(KillingComponent::Target::Player)) return;

	--m_Health;
	UpdateDisplay();

	ReceivedDamageEvent tookDmgEvent{ this };
	m_TookDamageEvent.Notify(tookDmgEvent);
}

void digdug::HealthComponent::LinkTextComponent(minigin::TextComponent* comp, const std::string& message)
{
	if (comp == nullptr) return;

	m_Message = message;
	m_Display = comp;

	UpdateDisplay();
}

void digdug::HealthComponent::UpdateDisplay()
{
	if (m_Display == nullptr) return;

	try
	{
		std::stringstream ss{};
		ss << m_Message << m_Health;

		m_Display->SetText(ss.str());
	}
	catch (const std::exception& e)
	{
		std::cerr << "Health component: " << e.what() << "\n";
	}
}
