#include "GridMoveCmd.h"
#include <algorithm>
#include <RenderComponent.h>
#include <AimComponent.h>

digdug::GridMoveCmd::GridMoveCmd(minigin::GameObject* pActor, int playerID, DirtGrid& grid, float speed, bool canDig)
	: ActorCommand(pActor, playerID)
	, m_Grid{ &grid }
	, m_MovementSpeed{ speed }
	, m_CanDig{ canDig }
{
	GetActor().SetParent(&grid.GetOwner());
	SetGridPosition(m_PosInGrid);

	auto renderComp = GetActor().GetComponent<minigin::RenderComponent>();

	if (renderComp != nullptr)
	{
		m_ActorSize = renderComp->GetSize();
	}
	else
	{
		std::cerr << "WARNING: GridMoveCmd::GridMoveCmd, couldn't find a RenderComponent, size will be zero!\n";
	}

	m_AimComp = GetActor().GetComponent<digdug::AimComponent>();

	if (m_AimComp == nullptr)
	{
		std::cout << "INFO: GridMoveCmd::GridMoveCmd couldn't find AimComponent.\n";
	}
}

void digdug::GridMoveCmd::SetGridPosition(const glm::ivec2& gridPos)
{
	m_PosInGrid = gridPos;
	m_TargetGridPos = m_PosInGrid;
	GetActor().SetLocalPosition(m_Grid->GetCellLocalPos(m_PosInGrid));
}

void digdug::GridMoveCmd::ActorExecute(const minigin::InputContext& context, float deltaTime)
{
	if (m_LastFrame == context.frame) return;
	m_LastFrame = context.frame;

	glm::vec3 target = m_Grid->GetCellLocalPos(m_TargetGridPos);

	glm::vec3 pos = GetActor().GetLocalPosition();
	glm::vec3 toTarget = target - pos;

	float speed = m_MovementSpeed;

	if (!m_Grid->HasBeenDug(m_TargetGridPos) && m_CanDig)
	{
		speed /= 2;
	}

	float step = speed * deltaTime;

	if (glm::dot(toTarget, toTarget) <= step * step)
	{
		m_Grid->Dig(m_PosInGrid, m_TargetGridPos, 'x');

		pos = target;
		SelectNewTarget(context.axis);
		UpdateAimComponent(context.axis);
	}
	else
	{
		pos += glm::normalize(toTarget) * step;
	}

	const auto locked{ KeepInBounds(pos) };

	if (pos != locked)
	{
		m_TargetGridPos = m_PosInGrid;
		pos = locked;
	}

	GetActor().SetLocalPosition(pos);
}

glm::vec3 digdug::GridMoveCmd::KeepInBounds(const glm::vec3& pos) const
{
	auto max = glm::vec3(m_Grid->GetSize(), 0.f);
	
	glm::vec3 newPos{};
	newPos.x = std::clamp(pos.x, 0.f, max.x - m_ActorSize.x);
	newPos.y = std::clamp(pos.y, 0.f, max.y - m_ActorSize.y);

	return newPos;
}

void digdug::GridMoveCmd::SelectNewTarget(const glm::vec2& axis)
{
	glm::vec2 absAxis{};
	absAxis.x = glm::abs(axis.x);
	absAxis.y = glm::abs(axis.y);

	glm::ivec2 newDir{ 0.f, 0.f };

	if (absAxis.x < absAxis.y)
	{
		newDir.y = std::signbit(axis.y) ? -1 : 1;
	}

	if (absAxis.x > absAxis.y)
	{
		newDir.x = std::signbit(axis.x) ? -1 : 1;
	}

	m_PosInGrid = m_TargetGridPos;

	auto newTarget{ m_PosInGrid + newDir };

	if (!m_Grid->IsWallOpen(m_PosInGrid, newTarget) && !m_CanDig)
	{
		return;
	}

	m_TargetGridPos = newTarget;
}

void digdug::GridMoveCmd::UpdateAimComponent(const glm::vec2& axis)
{
	if (m_AimComp == nullptr) return;

	auto aim = glm::vec2(m_TargetGridPos - m_PosInGrid);

	if (m_TargetGridPos == m_PosInGrid)
	{
		aim = axis;
	}

	if (aim.x > 0)			m_AimComp->SetDirection(minigin::Direction::Right);
	else if (aim.x < 0)	m_AimComp->SetDirection(minigin::Direction::Left);
	else if (aim.y > 0)	m_AimComp->SetDirection(minigin::Direction::Down);
	else if (aim.y < 0)	m_AimComp->SetDirection(minigin::Direction::Up);
}
