#include "HealthComponent.h"

#include <sstream>
#include <iostream>

#include <TextComponent.h>
#include <Events.h>

#include <ServiceLocator.h>

void digdug::HealthComponent::Start()
{
	minigin::HitEvent event{ nullptr };
	m_HitEventHash = minigin::PlainEvent::CreateHashSDBM(event.GetEventName());

	UpdateDisplay();
}

void digdug::HealthComponent::OnNotify(const minigin::IEvent& event)
{
	if (event.GetEventHash() != m_HitEventHash)
	{
		return;
	}

	--m_Health;

	minigin::ServiceLocator::GetSoundSystem()->Play("Sound/GameOver.mp3", 0.01f);

	UpdateDisplay();
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
	if (m_Display == nullptr)
	{
		return;
	}

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
