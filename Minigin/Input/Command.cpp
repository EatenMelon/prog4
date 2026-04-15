#include "Command.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include "ScoreComponent.h"

dae::MoveCommand::MoveCommand(GameObject* pActor, float movementSpeed)
	: ActorCommand(pActor)
	, m_MovementSpeed{ movementSpeed }
{

}

void dae::MoveCommand::Execute(const InputContext& context, float deltaTime)
{

	glm::vec3 pos = GetActor().GetLocalPosition();

	pos.x += context.axis.x * m_MovementSpeed * deltaTime;
	pos.y += context.axis.y * m_MovementSpeed * deltaTime;

	GetActor().SetLocalPosition(pos);
}

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

dae::ScoreCommand::ScoreCommand(GameObject* pActor, ScoreComponent* pScore)
	: ActorCommand(pActor)
	, m_pScore{ pScore }
{

}

void dae::ScoreCommand::Execute(const InputContext&, float)
{
	if (m_pScore == nullptr)
	{
		auto ScoreComp = GetActor().GetComponent<ScoreComponent>();

		if (ScoreComp == nullptr) return;

		m_pScore = ScoreComp;
	}

	m_pScore->AddPoints(100);
}