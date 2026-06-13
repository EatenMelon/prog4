#include "Attack.h"

#include <SceneManager.h>
#include <RenderComponent.h>

#include <AimComponent.h>
#include <Enemy.h>
#include <FireBreath.h>
#include <Hitbox.h>
#include <KillingComponent.h>

#include <ResourceLocator.h>

void digdug::FireAttack::Execute(Enemy* attacker)
{
	auto& scene = minigin::SceneManager::GetInstance().GetActiveScene();

	auto fireObj = std::make_unique<minigin::GameObject>();
	auto renderComp = fireObj->AddComponent<minigin::RenderComponent>();
	if (renderComp == nullptr)
	{
		std::cerr << "ERROR: FireAttack::Execute, fire breath could not be rendered!\n";
		return;
	}

	const auto location = ResourceLocator::GetInstance().GetResource(ResourceLocator::Type::Sprite, "atk/fire");
	renderComp->SetTexture(location);

	fireObj->AddComponent<digdug::AimComponent>();

	auto hitbox = fireObj->AddComponent<minigin::Hitbox>();
	if (hitbox == nullptr)
	{
		std::cerr << "ERROR: FireAttack::Execute, could not add hitbox component\n";
		return;
	}

	auto killComp = fireObj->AddComponent<digdug::KillingComponent>();
	if (killComp == nullptr)
	{
		std::cerr << "ERROR: FireAttack::Execute, could not add killing component\n";
		return;
	}
	killComp->AddTarget(KillingComponent::Target::Player);

	auto fireBreathComp = fireObj->AddComponent<digdug::FireBreath>();
	if (fireBreathComp == nullptr)
	{
		std::cerr << "ERROR: FireAttack::Execute, fire breath would not be added as a component!\n";
		return;
	}
	fireBreathComp->SetCaster(attacker);

	scene.Add(std::move(fireObj));
}
