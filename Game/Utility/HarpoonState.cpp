#include "HarpoonState.h"
#include <Hitbox.h>

#include <Harpoon.h>
#include <Inflatable.h>
#include <AimComponent.h>

#include <DirtGrid.h>

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

	const auto dirVec = GetHarpoon()->GetAimComponent()->GetDirectionAsVector();

	if (dirVec == glm::ivec2(0)) return;

	auto pos = m_Harpoon->GetOwner().GetLocalPosition();
	pos = glm::vec3(selectedSize, 0) * glm::vec3(dirVec, 0);

	m_Harpoon->GetOwner().SetLocalPosition(pos);
}

void digdug::HarpoonState::UpdateHitbox()
{
	auto hitbox = GetHarpoon()->GetHitbox();
	auto size = GetHarpoonSize();

	hitbox->SetBounds(size.x, size.y);
}

digdug::HarpoonIdleState::HarpoonIdleState(Harpoon* harpoon)
	: HarpoonState(harpoon)
{
	auto hitbox = GetHarpoon()->GetHitbox();

	hitbox->Enable(false);
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonIdleState::StartShoot()
{
	return std::make_unique<HarpoonShootState>(GetHarpoon());
}

digdug::HarpoonShootState::HarpoonShootState(Harpoon* harpoon)
	: HarpoonState(harpoon)
{
	auto hitbox = GetHarpoon()->GetHitbox();

	hitbox->Enable(true);
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonShootState::Update(float deltaTime, DirtGrid* grid)
{
	UpdatePosition();
	UpdateHitbox();

	Extend(m_LaunchSpeed * deltaTime);

	const float maxExtend{ 1.f };
	float extendLimit{ maxExtend };

	if (grid != nullptr)
	{
		const auto direction = GetHarpoon()->GetAimComponent()->GetDirectionAsVector();

		auto pos = GetHarpoon()->GetUser()->GetLocalPosition();
		pos += glm::vec3(GetHarpoon()->GetUserSize() / 2.f, 0);

		const auto gridPosUser = grid->GetPosInGrid(pos);
		const int harpoonLength{ 2 };

		for (int ext{ harpoonLength }; ext > 0; --ext)
		{
			const auto cellInFront = gridPosUser + direction * ext;

			if (grid->HasBeenDug(cellInFront)) continue;

			extendLimit -= maxExtend / harpoonLength;
		}
	}

	if (GetExtend() >= extendLimit)
	{
		// retract
		return std::make_unique<HarpoonRetractState>(GetHarpoon(), GetExtend());
	}

	return  nullptr;
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonShootState::OnAttach(Inflatable* inflatable)
{
	return std::make_unique<HarpoonPumpingState>(GetHarpoon(), GetExtend(), inflatable);
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonShootState::StartRetract()
{
	return std::make_unique<HarpoonRetractState>(GetHarpoon(), GetExtend());
}

digdug::HarpoonPumpingState::HarpoonPumpingState(Harpoon* harpoon, float extend, Inflatable* inflatable)
	: HarpoonState(harpoon, extend)
	, m_Inflatable{ inflatable }
{
	m_Inflatable->PoppedEvent().Subscribe(this);
	m_PumpEvent.Subscribe(m_Inflatable);

	InflatablePoppedEvent event{};
	m_PoppedEventHash = event.GetEventHash();
}

digdug::HarpoonPumpingState::~HarpoonPumpingState()
{
	m_Inflatable->PoppedEvent().UnSubscribe(this);
	m_PumpEvent.UnSubscribe(m_Inflatable);
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonPumpingState::Update(float deltaTime, DirtGrid*)
{
	UpdatePosition();
	UpdateHitbox();
	
	if (m_TryRetract && m_TimeUntilRetract <= 0.f)
	{
		m_RetractNextFrame = true;
	}
	else if (m_TryRetract)
	{
		m_TimeUntilRetract -= deltaTime;
	}

	if (m_RetractNextFrame)
	{
		PumpDetachEvent event{};
		m_PumpEvent.Notify(event);

		return std::make_unique<HarpoonRetractState>(GetHarpoon(), GetExtend());
	}

	m_TimeUntilNextPump -= deltaTime;

	if (m_TimeUntilNextPump <= 0.f)
	{
		// pump
		PumpInflatableEvent event{};
		m_PumpEvent.Notify(event);

		m_TimeUntilNextPump = m_PumpDelay;
	}

	return nullptr;
}

void digdug::HarpoonPumpingState::OnNotify(const minigin::IEvent& event)
{
	if (m_PoppedEventHash != event.GetEventHash()) return;

	m_RetractNextFrame = true;
	m_PumpEvent.UnSubscribe(m_Inflatable);
	m_Inflatable->PoppedEvent().UnSubscribe(this);
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonPumpingState::StartShoot()
{
	m_TryRetract = false;

	PumpInflatableEvent event{};
	m_PumpEvent.Notify(event);
	m_TimeUntilNextPump = m_PumpDelay;

	return nullptr;
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonPumpingState::StartRetract()
{
	m_TryRetract = true;
	m_TimeUntilRetract = m_RetractDelay;

	return nullptr;
}

digdug::HarpoonRetractState::HarpoonRetractState(Harpoon* harpoon, float extend)
	: HarpoonState(harpoon, extend)
{
	auto hitbox = GetHarpoon()->GetHitbox();

	hitbox->Enable(false);
}

std::unique_ptr<digdug::HarpoonState> digdug::HarpoonRetractState::Update(float deltaTime, DirtGrid*)
{
	UpdatePosition();
	UpdateHitbox();

	Retract(m_RetractSpeed * deltaTime);

	if (GetExtend() <= 0.f)
	{
		// idle
		return std::make_unique<HarpoonIdleState>(GetHarpoon());
	}

	return nullptr;
}
