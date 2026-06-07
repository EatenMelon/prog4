#include "HarpoonState.h"
#include <Harpoon.h>
#include <AimComponent.h>
#include <Hitbox.h>

std::unordered_map<minigin::Direction, glm::ivec2> digdug::HarpoonState::m_PositionMap
{
	{minigin::Direction::Up,	glm::ivec2{0, -1}},
	{minigin::Direction::Down,	glm::ivec2{0, 1}},
	{minigin::Direction::Left,	glm::ivec2{-1, 0}},
	{minigin::Direction::Right,	glm::ivec2{1, 0}}
};

digdug::HarpoonState::HarpoonState(Harpoon* harpoon, float extend)
	: m_Harpoon{ harpoon }
	, m_Extended{ extend }
{
	m_Extended = std::clamp(m_Extended, 0.f, 1.f);
}

glm::vec2 digdug::HarpoonState::GetHarpoonSize() const
{
	auto aimDir = m_Harpoon->GetAimComponent()->GetDirection();
	const auto spriteSize = m_Harpoon->GetSize();

	glm::vec2 finalSize{};

	switch (aimDir)
	{
	case minigin::Direction::Up:
	case minigin::Direction::Down:
		finalSize.x = spriteSize.y;
		finalSize.y = spriteSize.x * m_Extended;
		break;

	default:
		finalSize = spriteSize;
		finalSize.x *= m_Extended;
		break;
	}

	return finalSize;
}

void digdug::HarpoonState::UpdatePosition()
{
	const auto currentDir = m_Harpoon->GetAimComponent()->GetDirection();

	if (m_LastDir == currentDir) return;
	m_LastDir = currentDir;

	glm::vec2 selectedSize{};

	switch (currentDir)
	{
	case minigin::Direction::Up:
	case minigin::Direction::Left:
		selectedSize = GetHarpoonSize();
		break;

	case minigin::Direction::Down:
	case minigin::Direction::Right:
		selectedSize = m_Harpoon->GetUserSize();
		break;

	default: break;
	}

	auto itr = m_PositionMap.find(currentDir);
	if (itr == m_PositionMap.end()) return;

	auto pos = m_Harpoon->GetOwner().GetLocalPosition();
	pos = glm::vec3(selectedSize, 0) * glm::vec3(itr->second, 0);

	m_Harpoon->GetOwner().SetLocalPosition(pos);
}

void digdug::HarpoonState::UpdateHitbox()
{
	auto hitbox = GetHarpoon()->GetHitbox();
	auto size = GetHarpoonSize();

	hitbox->SetBounds(size.x, size.y);
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonIdleState::Update(float)
{
	auto hitbox = GetHarpoon()->GetHitbox();

	if (hitbox->Enabled())
	{
		hitbox->Enable(false);
	}

	return nullptr;
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonIdleState::StartShoot()
{
	return nullptr;
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonIdleState::StartRetract()
{
	return nullptr;
}
