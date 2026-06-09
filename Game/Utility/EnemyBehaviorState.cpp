#include "EnemyBehaviorState.h"

#include <GameObject.h>
#include <RenderComponent.h>

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

digdug::EnemyWanderState::EnemyWanderState(Enemy* enemy, DirtGrid* grid)
	: EnemyBehaviorState(enemy, grid)
{
	m_CurrentGridPos = GetEnemyGridPos();
	m_TargetGridPos = m_CurrentGridPos;

	SelectTarget();
	GetEnemy()->BecomeDefault();

	m_TimeUntilNewAction = m_TryNewActionDelay;
}

std::unique_ptr<digdug::EnemyBehaviorState> digdug::EnemyWanderState::Update(float deltaTime)
{
	if (m_TimeUntilNewAction <= 0.f)
	{
		const NewAction tryAction{ static_cast<NewAction>(rand() % m_NumActions) };

		switch (tryAction)
		{
		case digdug::EnemyWanderState::NewAction::Attack:

			if (!CanAttack()) break;

			GetEnemy()->DoAttack();

			return std::make_unique<EnemyAttackingState>(GetEnemy(), GetGrid());
			break;

		case digdug::EnemyWanderState::NewAction::TurnGhost:
			return std::make_unique<EnemyGhostState>(GetEnemy(), GetGrid());
			break;

		default:
			break;
		}

		m_TimeUntilNewAction = m_TryNewActionDelay;
	}
	else
	{
		m_TimeUntilNewAction -= deltaTime;
	}

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

std::unique_ptr<digdug::EnemyBehaviorState> digdug::EnemyWanderState::OnInflatedEnter(minigin::GameObject*)
{
	return std::make_unique<EnemyFrozenState>(GetEnemy(), GetGrid());
}

void digdug::EnemyWanderState::SelectTarget()
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

	if (newDir.x < 0)
	{
		GetEnemy()->GetAimComponent()->SetDirection(minigin::Direction::Left);
	}
	else if (newDir.x > 0)
	{
		GetEnemy()->GetAimComponent()->SetDirection(minigin::Direction::Right);
	}
}

bool digdug::EnemyWanderState::CanAttack()
{
	const auto dir = GetEnemy()->GetAimComponent()->GetDirectionAsVector();

	return GetGrid()->HasBeenDug(m_CurrentGridPos + dir);
}

digdug::EnemyFrozenState::EnemyFrozenState(Enemy* enemy, DirtGrid* grid)
	: EnemyBehaviorState(enemy, grid)
{
	GetEnemy()->BecomeDefault();
}

std::unique_ptr<digdug::EnemyBehaviorState> digdug::EnemyFrozenState::OnDeflatedEnter()
{
	return std::make_unique<EnemyWanderState>(GetEnemy(), GetGrid());
}

digdug::EnemyAttackingState::EnemyAttackingState(Enemy* enemy, DirtGrid* grid)
	: EnemyBehaviorState(enemy, grid)
{
	GetEnemy()->BecomeDefault();
}

std::unique_ptr<digdug::EnemyBehaviorState> digdug::EnemyAttackingState::OnAttackEnded()
{
	return std::make_unique<EnemyWanderState>(GetEnemy(), GetGrid());
}

std::unique_ptr<digdug::EnemyBehaviorState> digdug::EnemyAttackingState::OnInflatedEnter(minigin::GameObject*)
{
	return std::make_unique<EnemyFrozenState>(GetEnemy(), GetGrid());
}

digdug::EnemyGhostState::EnemyGhostState(Enemy* enemy, DirtGrid* grid)
	: EnemyBehaviorState(enemy, grid)
{
	// select a gameobject (player) as a target to fly toward
	GetEnemy()->BecomeGhost();

	m_Target = GetEnemy()->GetRandomTarget();

	if (m_Target == nullptr) return;

	const auto renderComp = m_Target->GetComponent<minigin::RenderComponent>();

	if (renderComp != nullptr)
	{
		m_TargetSize = renderComp->GetSize();
	}

	m_CellOfTransformation = GetEnemyGridPos();
}

std::unique_ptr<digdug::EnemyBehaviorState> digdug::EnemyGhostState::Update(float deltaTime)
{
	if (m_Target == nullptr)
	{
		return std::make_unique<EnemyWanderState>(GetEnemy(), GetGrid());
	}

	m_TimeUntilRedirect -= deltaTime;
	if (m_TimeUntilRedirect <= 0.f)
	{
		if (EneteredNewTunnel())
		{
			return std::make_unique<EnemyWanderState>(GetEnemy(), GetGrid());
		}

		m_TimeUntilRedirect = m_RedirectionDelay;
		ChangeDirection();
	}

	// fly towards target player until you end up in a tunnel
	auto pos = GetEnemy()->GetOwner().GetLocalPosition();
	pos += glm::vec3(m_Direction * (GetEnemy()->GetMovementSpeed() / 2.f) * deltaTime, 0.f);

	GetEnemy()->GetOwner().SetLocalPosition(pos);

	return nullptr;
}

std::unique_ptr<digdug::EnemyBehaviorState> digdug::EnemyGhostState::OnInflatedEnter(minigin::GameObject* pumpUser)
{
	glm::ivec2 gridPosUser{};
	minigin::Direction aimDirUser{};

	try
	{
		glm::vec2 userSize{};

		auto renderComp = pumpUser->GetComponent<minigin::RenderComponent>();
		if (renderComp != nullptr)
		{
			userSize = renderComp->GetSize();
		}
		const auto posUser = pumpUser->GetLocalPosition() + glm::vec3(userSize / 2.f, 0.f);
		gridPosUser = GetGrid()->GetPosInGrid(posUser);

		auto aimComp = pumpUser->GetComponent<AimComponent>();
		if (aimComp != nullptr)
		{
			aimDirUser = aimComp->GetDirection();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: EnemyGhostState::OnInflatedEnter, " << e.what() << "\n";
		return nullptr;
	}

	auto enemyGridPos = GetEnemyGridPos();

	switch (aimDirUser)
	{
	case minigin::Direction::Up:
	case minigin::Direction::Down:
		enemyGridPos.x = gridPosUser.x;
		break;

	default:
		enemyGridPos.y = gridPosUser.y;
		break;
	}

	GetEnemy()->GetOwner().SetLocalPosition(GetGrid()->GetCellLocalPos(enemyGridPos));

	return std::make_unique<EnemyFrozenState>(GetEnemy(), GetGrid());
}

void digdug::EnemyGhostState::ChangeDirection()
{
	const auto targetPos = m_Target->GetLocalPosition() - glm::vec3(m_TargetSize / 2.f, 0.f);
	const auto startPos = GetEnemy()->GetOwner().GetLocalPosition() - glm::vec3(GetEnemy()->GetSize() / 2.f, 0.f);

	const auto toTarget = targetPos - startPos;

	m_Direction = glm::normalize(toTarget);
}

bool digdug::EnemyGhostState::EneteredNewTunnel()
{
	const auto gridPos = GetEnemyGridPos();

	if (gridPos == m_CellOfTransformation) return false;
	if (!GetGrid()->HasBeenDug(gridPos)) return false;

	GetEnemy()->GetOwner().SetLocalPosition(GetGrid()->GetCellLocalPos(gridPos));

	return true;
}
