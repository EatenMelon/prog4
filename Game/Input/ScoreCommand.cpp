#include "ScoreCommand.h"

#include <GameObject.h>
#include <ScoreComponent.h>

digdug::ScoreCommand::ScoreCommand(minigin::GameObject* pActor, ScoreComponent* pScore)
	: ActorCommand(pActor)
	, m_pScore{ pScore }
{

}

void digdug::ScoreCommand::Execute(const minigin::InputContext&, float)
{
	if (m_pScore == nullptr)
	{
		auto ScoreComp = GetActor().GetComponent<ScoreComponent>();

		if (ScoreComp == nullptr) return;

		m_pScore = ScoreComp;
	}

	m_pScore->AddPoints(100);
}