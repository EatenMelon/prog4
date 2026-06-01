#pragma once
#include <Command.h>

namespace digdug
{
	class ScoreComponent;
	class ScoreCommand final : public minigin::ActorCommand
	{
	public:
		ScoreCommand(minigin::GameObject* pActor, ScoreComponent* pScore);

		void Execute(const minigin::InputContext& context, float deltaTime) override;

	private:
		ScoreComponent* m_pScore;

	};
}