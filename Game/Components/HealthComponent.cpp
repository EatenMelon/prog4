#include "HealthComponent.h"

#include <sstream>

#include <GameObject.h>
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

	minigin::ServiceLocator::GetSoundSystem()->Play("Sound/GameOver.mp3", 1.f);

	UpdateDisplay();
}

void digdug::HealthComponent::LinkTextComponent(minigin::TextComponent* comp)
{
	if (comp == nullptr) return;

	m_Display = comp;
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
		ss << "Health: " << m_Health;

		m_Display->SetText(ss.str());
	}
	catch (const std::exception& e)
	{
		std::cout << "Health component: " << e.what() << "\n";
	}
}
