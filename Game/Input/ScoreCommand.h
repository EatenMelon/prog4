#pragma once
#include <Command.h>

namespace digdug
{
	class ScoreComponent;
	class ScoreCommand final : public minigin::ActorCommand
	{
	public:
		ScoreCommand(minigin::GameObject* pActor, int playerID, ScoreComponent* pScore);

	protected:
		void ActorExecute(const minigin::InputContext& context, float deltaTime) override;

	private:
		ScoreComponent* m_pScore;

	};
}