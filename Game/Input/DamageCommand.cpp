#include "DamageCommand.h"

#include <GameObject.h>
#include <HealthComponent.h>
#include <ServiceLocator.h>

dae::DamageCommand::DamageCommand(GameObject* pActor, HealthComponent* pHealth)
	: ActorCommand(pActor)
	, m_pHealth{ pHealth }
{

}

void dae::DamageCommand::Execute(const InputContext&, float)
{
	if (m_pHealth == nullptr)
	{
		auto healthComp = GetActor().GetComponent<HealthComponent>();

		if (healthComp == nullptr) return;

		m_pHealth = healthComp;
	}

	m_pHealth->Damage(1);

	ServiceLocator::GetSoundSystem()->Play("Sound/GameOver.mp3", 1.f);
}