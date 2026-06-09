#include "Attack.h"

#include <SceneManager.h>
#include <RenderComponent.h>

#include <AimComponent.h>
#include <Enemy.h>
#include <FireBreath.h>

void digdug::FireAttack::Execute(Enemy* attacker)
{
	try
	{
		auto& scene = minigin::SceneManager::GetInstance().GetActiveScene();

		auto fireObj = std::make_unique<minigin::GameObject>();

		auto renderComp = fireObj->AddComponent<minigin::RenderComponent>();
		renderComp->SetTexture("Sprites/Attacks/Fire.png");

		fireObj->AddComponent<digdug::AimComponent>();

		auto fireBreathComp = fireObj->AddComponent<digdug::FireBreath>();
		fireBreathComp->SetCaster(attacker);

		scene.Add(std::move(fireObj));
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: FireAttack::Execute, " << e.what() << "\n";
	}
}
