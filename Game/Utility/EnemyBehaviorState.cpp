#include "EnemyBehaviorState.h"
#include <GameObject.h>

#include <Enemy.h>
#include <DirtGrid.h>
#include <AimComponent.h>

glm::ivec2 digdug::EnemyBehaviorState::GetEnemyGridPos()
{
	const auto pos = GetEnemy()->GetOwner().GetLocalPosition();
	const auto size = GetEnemy()->GetSize();
	const auto center = pos + glm::vec3(size / 2.f, 0);

	return GetGrid()->GetPosInGrid(center);
}

digdug::EnemyRoamingState::EnemyRoamingState(Enemy* enemy, DirtGrid* grid)
	: EnemyBehaviorState(enemy, grid)
{
	m_CurrentGridPos = GetEnemyGridPos();
	m_TargetGridPos = m_CurrentGridPos;

	SelectTarget();
}

std::unique_ptr<digdug::EnemyBehaviorState> digdug::EnemyRoamingState::Update(float deltaTime)
{
	glm::vec3 target = GetGrid()->GetCellLocalPos(m_TargetGridPos);

	glm::vec3 pos = GetEnemy()->GetOwner().GetLocalPosition();
	glm::vec3 toTarget = target - pos;

	const float step = GetEnemy()->GetMovementSpeed() * deltaTime;

	if (glm::dot(toTarget, toTarget) <= step * step)
	{
		GetEnemy()->GetOwner().SetLocalPosition(GetGrid()->GetCellLocalPos(m_TargetGridPos));
		SelectTarget();
	}
	else
	{
		pos += glm::normalize(toTarget) * step;
	}

	GetEnemy()->GetOwner().SetLocalPosition(pos);

	return nullptr;
}

std::unique_ptr<digdug::EnemyBehaviorState> digdug::EnemyRoamingState::OnInflatedEnter()
{
	return nullptr;
}

void digdug::EnemyRoamingState::SelectTarget()
{
	std::vector<glm::ivec2> directions{};

	directions.push_back(AimComponent::GetDirectionAsVector(minigin::Direction::Up));
	directions.push_back(AimComponent::GetDirectionAsVector(minigin::Direction::Down));
	directions.push_back(AimComponent::GetDirectionAsVector(minigin::Direction::Left));
	directions.push_back(AimComponent::GetDirectionAsVector(minigin::Direction::Right));

	const auto lastDir{ m_TargetGridPos - m_CurrentGridPos };
	m_CurrentGridPos = m_TargetGridPos;

	directions.erase
	(
		std::remove_if
		(
			directions.begin(),
			directions.end(),
			[&](const auto& dir)
			{
				const auto nextPos{ m_CurrentGridPos + dir };
				const bool isBlocked{ !GetGrid()->IsWallOpen(m_CurrentGridPos, nextPos) };
				const bool isBackwards{ dir == -lastDir };
				return isBlocked || isBackwards;
			}
		),
		directions.end()
	);

	if (directions.size() <= 0)
	{
		// maybe force into ghost here
		m_TargetGridPos = m_CurrentGridPos;
		return;
	}

	const size_t randomIdx{ rand() % directions.size() };

	const auto newDir = directions[randomIdx];
	m_TargetGridPos = m_CurrentGridPos + newDir;
}
