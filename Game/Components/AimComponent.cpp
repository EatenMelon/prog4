#include "AimComponent.h"
#include <RenderComponent.h>
#include <GameObject.h>

const std::unordered_map<minigin::Direction, glm::ivec2> digdug::AimComponent::m_ToVecMap
{
	{minigin::Direction::Up,	glm::ivec2{0, -1}},
	{minigin::Direction::Down,	glm::ivec2{0, 1}},
	{minigin::Direction::Left,	glm::ivec2{-1, 0}},
	{minigin::Direction::Right,	glm::ivec2{1, 0}},
	{minigin::Direction::None,	glm::ivec2{0, 0}}
};

void digdug::AimComponent::Start()
{
	m_RenderComp = GetOwner().GetComponent<minigin::RenderComponent>();

	if (m_RenderComp == nullptr)
	{
		throw std::runtime_error("AimComponent requires a RenderComponent!");
	}
}

void digdug::AimComponent::Update(float)
{
	if (!m_IsDirty) return;

	switch (m_AimDirection)
	{
	case minigin::Direction::Left:
	case minigin::Direction::Right:
		if (!m_LockedAxis.x) break;
		m_AimDirection = m_LastDirection;
		break;

	default:
		if (!m_LockedAxis.y) break;
		m_AimDirection = m_LastDirection;
		break;
	}

	constexpr float turnAngle{ 90.f };

	switch (m_AimDirection)
	{
	case minigin::Direction::Up:
		m_RenderComp->SetHorizontalFlip(false);
		m_RenderComp->SetRotation(turnAngle * 3);
		break;

	case minigin::Direction::Down:
		m_RenderComp->SetHorizontalFlip(false);
		m_RenderComp->SetRotation(turnAngle);
		break;

	case minigin::Direction::Left:
		m_RenderComp->SetHorizontalFlip(true);
		m_RenderComp->SetRotation(turnAngle * 0);
		break;

	case::minigin::Direction::None:
	case minigin::Direction::Right:
		m_RenderComp->SetHorizontalFlip(false);
		m_RenderComp->SetRotation(turnAngle * 0);
		break;
	}
}

void digdug::AimComponent::SetDirection(minigin::Direction direction)
{
	m_LastDirection = m_AimDirection;
	m_AimDirection = direction;
	m_IsDirty = true;
}

glm::ivec2 digdug::AimComponent::GetDirectionAsVector() const
{
	return GetDirectionAsVector(m_AimDirection);
}

glm::ivec2 digdug::AimComponent::GetDirectionAsVector(minigin::Direction dir)
{
	auto itr = m_ToVecMap.find(dir);

	if (itr == m_ToVecMap.end()) return glm::ivec2(0);

	return itr->second;
}

void digdug::AimComponent::LockAxis(const glm::bvec2& axisLocks)
{
	m_LockedAxis = axisLocks;
	m_IsDirty = true;

	switch (m_LastDirection)
	{
	case minigin::Direction::Left:
	case minigin::Direction::Right:
		if (!m_LockedAxis.x) break;
		m_LastDirection = minigin::Direction::Up;
		break;

	default:
		if (!m_LockedAxis.y) break;
		m_LastDirection = minigin::Direction::Right;
		break;
	}
}
