#include "Enemy.h"

#include <algorithm>

#include <GameObject.h>
#include <ResourceManager.h>
#include <RenderComponent.h>

#include <Inflatable.h>
#include <AimComponent.h>

void digdug::Enemy::Start()
{
	m_RenderComp = GetOwner().GetComponent<minigin::RenderComponent>();

	if (m_RenderComp == nullptr)
	{
		throw std::runtime_error("No render component found!");
	}

	m_Inflatable = GetOwner().GetComponent<Inflatable>();

	if (m_Inflatable == nullptr)
	{
		throw std::runtime_error("No inflatable component found!");
	}

	m_AimComponent = GetOwner().GetComponent<AimComponent>();

	if (m_AimComponent == nullptr)
	{
		throw std::runtime_error("Enemy cannot be aimed!");
	}

	m_WasInflated = m_Inflatable->IsInflated();
}

void digdug::Enemy::Update(float)
{
	const bool isInflated = m_Inflatable->IsInflated();
	if (m_WasInflated == isInflated) return;

	if (!m_WasInflated && isInflated)
	{
		// inflate enter
		EnemyInflatedEvent event{ m_Inflatable->GetPumpUser() };
		m_OnInflatedEnter.Notify(event);
	}
	else if (m_WasInflated && !isInflated)
	{
		// deflate enter
		EnemyDeflatedEvent event{};
		m_OnDeflatedEnter.Notify(event);
	}

	m_WasInflated = isInflated;
}

void digdug::Enemy::BecomeDefault()
{
	m_RenderComp->SetTexture(m_DefaultSprite);
}

void digdug::Enemy::BecomeGhost()
{
	m_RenderComp->SetTexture(m_GhostSprite);
}

void digdug::Enemy::DoAttack()
{

	if (m_Attack == nullptr)
	{
		AttackEndedEvent event{};
		m_OnEndAttack.Notify(event);
		return;
	}

	m_Attack->Execute(this);
}

void digdug::Enemy::SetDefaultSprite(const std::string& path)
{
	m_DefaultSprite = minigin::ResourceManager::GetInstance().LoadTexture(path);
}

void digdug::Enemy::SetGhostSprite(const std::string& path)
{
	m_GhostSprite = minigin::ResourceManager::GetInstance().LoadTexture(path);
}

void digdug::Enemy::SetAttack(std::shared_ptr<Attack> attack)
{
	m_Attack = attack;
}

void digdug::Enemy::OnNotify(const minigin::IEvent& event)
{
	m_OnEndAttack.Notify(event);
}

glm::vec2 digdug::Enemy::GetSize()
{
	return m_RenderComp->GetSize();
}

void digdug::Enemy::AddPossibleTarget(minigin::GameObject* obj)
{
	const auto itr = std::find(m_PossibleTargets.begin(), m_PossibleTargets.end(), obj);

	if (itr != m_PossibleTargets.end()) return;

	m_PossibleTargets.push_back(obj);
}

minigin::GameObject* digdug::Enemy::GetRandomTarget()
{
	if (m_PossibleTargets.empty()) return nullptr;

	const size_t randomIdx{ rand() % m_PossibleTargets.size() };

	return m_PossibleTargets[randomIdx];
}
