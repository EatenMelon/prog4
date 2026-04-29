#pragma once
#include <Command.h>

namespace dae
{
	class ScoreComponent;
	class ScoreCommand final : public ActorCommand
	{
	public:
		ScoreCommand(GameObject* pActor, ScoreComponent* pScore);

		void Execute(const InputContext& context, float deltaTime) override;

	private:
		ScoreComponent* m_pScore;

	};
}