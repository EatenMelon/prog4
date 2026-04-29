#pragma once
#include <Command.h>


namespace dae
{
	class HealthComponent;
	class DamageCommand final : public ActorCommand
	{
	public:
		DamageCommand(GameObject* pActor, HealthComponent* pHealth);

		void Execute(const InputContext& context, float deltaTime) override;

	private:
		HealthComponent* m_pHealth;

	};
}
