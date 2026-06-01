#include "DamageCommand.h"

#include <GameObject.h>
#include <HealthComponent.h>
#include <ServiceLocator.h>

digdug::DamageCommand::DamageCommand(minigin::GameObject* pActor, HealthComponent* pHealth)
	: ActorCommand(pActor)
	, m_pHealth{ pHealth }
{

}

void digdug::DamageCommand::Execute(const minigin::InputContext&, float)
{
	if (m_pHealth == nullptr)
	{
		auto healthComp = GetActor().GetComponent<HealthComponent>();

		if (healthComp == nullptr) return;

		m_pHealth = healthComp;
	}

	m_pHealth->Damage(1);

	minigin::ServiceLocator::GetSoundSystem()->Play("Sound/GameOver.mp3", 1.f);
}