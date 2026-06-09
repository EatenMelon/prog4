#include "FireBreath.h"

#include <RenderComponent.h>
#include <GameObject.h>

#include <AimComponent.h>
#include <Enemy.h>
#include <Attack.h>

void digdug::FireBreath::Start()
{
	if (m_Enemy == nullptr)
	{
		throw std::runtime_error("Fire breath cannot be fired without an enemy!");
	}

	auto enemyRenderComp = m_Enemy->GetOwner().GetComponent<minigin::RenderComponent>();

	auto renderComp = GetOwner().GetComponent<minigin::RenderComponent>();
	if (renderComp != nullptr)
	{
		renderComp->SetUniformScale(enemyRenderComp->GetUniformScale());
	}
	else
	{
		throw std::runtime_error("Fire attack needs a render components!");
	}

	auto aimComp = GetOwner().GetComponent<digdug::AimComponent>();
	if (aimComp != nullptr)
	{
		aimComp->SetDirection(m_Enemy->GetAimComponent()->GetDirection());
	}
	else
	{
		throw std::runtime_error("Fire attack needs a aim components!");
	}

	glm::vec3 addedDistance{};

	switch (aimComp->GetDirection())
	{
	case minigin::Direction::Left:
		addedDistance.x = -renderComp->GetSize().x;
		break;

	case minigin::Direction::Right:
		addedDistance.x = m_Enemy->GetSize().x;
		break;

	default:
		break;
	}

	m_OnAttackEnded.Subscribe(m_Enemy);

	GetOwner().SetLocalPosition(m_Enemy->GetOwner().GetWorldPosition() + addedDistance);
}

void digdug::FireBreath::Update(float deltaTime)
{
	m_LifeTime -= deltaTime;

	if (m_LifeTime > 0.f) return;

	AttackEndedEvent event{};
	m_OnAttackEnded.Notify(event);

	m_OnAttackEnded.UnSubscribe(m_Enemy);

	GetOwner().Destroy();
}
