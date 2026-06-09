#include "EnemyBehavior.h"
#include <GameObject.h>

#include <Enemy.h>
#include <DirtGrid.h>

void digdug::EnemyBehavior::Start()
{
	m_Enemy = GetOwner().GetComponent<Enemy>();

	if (m_Enemy == nullptr)
	{
		throw std::runtime_error("Cannot give enemy behavior to non-enemy!");
	}

	if (m_Grid == nullptr)
	{
		throw std::runtime_error("Enemies can only move within a grid!");
	}

	m_Enemy->OnInflatedEnter().Subscribe(this);
	m_Enemy->OnDeflatedEnter().Subscribe(this);

	EnemyInflatedEvent inflateEvent{ nullptr };
	m_InflatedEnetrHash = inflateEvent.GetEventHash();

	EnemyDeflatedEvent deflateEvent{};
	m_DeflatedEnetrHash = deflateEvent.GetEventHash();

	m_State = std::make_unique<EnemyWanderState>(m_Enemy, m_Grid);
}

void digdug::EnemyBehavior::Update(float deltaTime)
{
	auto newState = m_State->Update(deltaTime);

	if (newState == nullptr) return;
	m_State = std::move(newState);
}

void digdug::EnemyBehavior::OnNotify(const minigin::IEvent& event)
{
	if (event.GetEventHash() == m_InflatedEnetrHash)
	{
		const auto& inflatedEvent = static_cast<const EnemyInflatedEvent&>(event);
		auto newState = m_State->OnInflatedEnter(inflatedEvent.GetPumpUser());

		if (newState == nullptr) return;
		m_State = std::move(newState);
	}

	if (event.GetEventHash() == m_DeflatedEnetrHash)
	{
		auto newState = m_State->OnDeflatedEnter();

		if (newState == nullptr) return;
		m_State = std::move(newState);
	}
}

void digdug::EnemyBehavior::SetPositionInGrid(const glm::ivec2& pos)
{
	GetOwner().SetLocalPosition(m_Grid->GetCellLocalPos(pos));
}
