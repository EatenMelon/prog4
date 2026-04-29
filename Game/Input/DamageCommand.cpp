#include "DamageCommand.h"

#include <GameObject.h>
#include <HealthComponent.h>

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
}