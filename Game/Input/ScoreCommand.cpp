#include "ScoreCommand.h"

#include <GameObject.h>
#include <ScoreComponent.h>

digdug::ScoreCommand::ScoreCommand(minigin::GameObject* pActor, int playerID, ScoreComponent* pScore)
	: ActorCommand(pActor, playerID)
	, m_pScore{ pScore }
{

}

void digdug::ScoreCommand::ActorExecute(const minigin::InputContext&, float)
{
	if (m_pScore == nullptr)
	{
		auto ScoreComp = GetActor().GetComponent<ScoreComponent>();

		if (ScoreComp == nullptr) return;

		m_pScore = ScoreComp;
	}

	m_pScore->AddPoints(100);
}