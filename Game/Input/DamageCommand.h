#pragma once
#include <Command.h>


namespace digdug
{
	class HealthComponent;
	class DamageCommand final : public minigin::ActorCommand
	{
	public:
		DamageCommand(minigin::GameObject* pActor, HealthComponent* pHealth);

		void Execute(const minigin::InputContext& context, float deltaTime) override;

	private:
		HealthComponent* m_pHealth;

	};
}
