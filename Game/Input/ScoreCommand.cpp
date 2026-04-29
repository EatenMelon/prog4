#include "ScoreCommand.h"

#include <GameObject.h>
#include <ScoreComponent.h>

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