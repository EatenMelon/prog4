#pragma once
#include <Command.h>


namespace digdug
{
	class HealthComponent;
	class DamageCommand final : public minigin::ActorCommand
	{
	public:
		DamageCommand(minigin::GameObject* pActor, int playerID, HealthComponent* pHealth);

	protected:
		void ActorExecute(const minigin::InputContext& context, float deltaTime) override;

	private:
		HealthComponent* m_pHealth;

	};
}
