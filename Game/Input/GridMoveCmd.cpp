#include "GridMoveCmd.h"
#include <algorithm>
#include <RenderComponent.h>

digdug::GridMoveCmd::GridMoveCmd(minigin::GameObject* pActor, int playerID, DirtGrid& grid, float speed, bool canDig)
	: ActorCommand(pActor, playerID)
	, m_Grid{ &grid }
	, m_MovementSpeed{ speed }
	, m_CanDig{ canDig }
{
	SetGridPosition(m_PosInGrid);

	auto renderComp = GetActor().GetComponent<minigin::RenderComponent>();

	if (renderComp != nullptr)
	{
		m_ActorSize = renderComp->GetSize();
	}
	else
	{
		std::cerr << "WARNING: GridMoveCmd couldn't find a RenderComponent, size will be zero!\n";
	}
}

void digdug::GridMoveCmd::SetGridPosition(const glm::ivec2& gridPos)
{
	m_PosInGrid = gridPos;
	GetActor().SetLocalPosition(m_Grid->GetCellWorldPos(m_PosInGrid));
}

void digdug::GridMoveCmd::ActorExecute(const minigin::InputContext& context, float deltaTime)
{
	// at worst you skip 1 frame
	// but prevents sonic behavior
	if (context.frame == m_LastFrame) return;
	m_LastFrame = context.frame;

	glm::vec3 pos = GetActor().GetLocalPosition();

	pos.x += m_MoveDirection.x * m_MovementSpeed * deltaTime;
	pos.y += m_MoveDirection.y * m_MovementSpeed * deltaTime;

	const auto limitedPos{ KeepInBounds(pos) };

	if (limitedPos != pos)
	{
		m_MoveDirection = glm::ivec2(0);
	}

	pos = limitedPos;

	auto newPosInGrid = m_Grid->GetPosInGrid(GetActor().GetLocalPosition());

	if (m_PosInGrid != newPosInGrid)
	{
		SetGridPosition(newPosInGrid);
		m_MoveDirection = glm::ivec2(0);
		std::cout << "Position in grid: [" << m_PosInGrid.x << ", " << m_PosInGrid.y << "]\n";
	}

	if (!m_CanDig)
	{
		std::cout << "Position in grid: [" << m_PosInGrid.x << ", " << m_PosInGrid.y << "]\n";
	}

	m_MoveDirection = ChooseDirection(context.axis);

	GetActor().SetLocalPosition(pos);
}

glm::vec3 digdug::GridMoveCmd::KeepInBounds(const glm::vec3& pos) const
{
	auto min = m_Grid->GetOwner().GetWorldPosition();
	auto max = glm::vec3(m_Grid->GetSize(), 0.f) + min;
	
	glm::vec3 newPos{};
	newPos.x = std::clamp(pos.x, min.x, max.x - m_ActorSize.x);
	newPos.y = std::clamp(pos.y, min.y, max.y - m_ActorSize.y);

	return newPos;
}

glm::ivec2 digdug::GridMoveCmd::ChooseDirection(const glm::vec2& axis)
{
	if (m_MoveDirection != glm::ivec2(0)) return m_MoveDirection;

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

	return newDir;
}
