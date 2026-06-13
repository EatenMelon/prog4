#include "Inflatable.h"

#include <ResourceManager.h>
#include <ServiceLocator.h>
#include <RenderComponent.h>
#include <Renderer.h>
#include <GameObject.h>
#include <Hitbox.h>

#include <HarpoonState.h>
#include <HealthComponent.h>
#include <ScoreComponent.h>
#include <DirtGrid.h>
#include <ResourceLocator.h>

void digdug::Inflatable::Start()
{
	m_RenderComp = GetOwner().GetComponent<minigin::RenderComponent>();

	if (m_SpriteSheet == nullptr)
	{
		std::cerr << "WARNING: InflatableComponent doesn't have a sprite sheet!\n";
	}

	PumpInflatableEvent pumpEvent{ nullptr, nullptr };
	m_PumpEventHash = pumpEvent.GetEventHash();

	PumpDetachEvent detachEvent{};
	m_DetachPumpEventHash = detachEvent.GetEventHash();
}

void digdug::Inflatable::Update(float deltaTime)
{
	if (m_Inflation >= m_MaxInflation - 1)
	{
		if (m_TimeUntilDestroy >= 0.f)
		{
			m_TimeUntilDestroy -= deltaTime;
			return;
		}

		GetOwner().Destroy();
		return;
	}

	if (m_Inflation < 0) return;
	if (!m_CanDefalte) return;

	m_TimeUntilDeflate -= deltaTime;

	if (m_TimeUntilDeflate <= 0)
	{
		--m_Inflation;
		m_TimeUntilDeflate = m_DeflationDelay;
	}

	m_RenderComp->Enable(m_Inflation < 0);
}

void digdug::Inflatable::Render() const
{
	if (m_Inflation < 0) return;
	if (m_SpriteSheet == nullptr) return;

	glm::vec2 pos = GetOwner().GetWorldPosition();
	glm::ivec2 spritePos{ m_Inflation, 0 };

	float scale{ 1 };

	if (m_RenderComp != nullptr)
	{
		scale = m_RenderComp->GetUniformScale();

		glm::vec2 spriteSize = m_SpriteSheet->GetSpriteSize();
		glm::vec2 size = m_RenderComp->GetSize();

		auto offset = size - spriteSize * scale;
		offset.x /= 2;

		pos += offset;
	}

	minigin::Renderer::GetInstance().RenderSprite(*m_SpriteSheet.get(), pos, spritePos, scale);
}

void digdug::Inflatable::OnNotify(const minigin::IEvent& event)
{
	if (m_DetachPumpEventHash == event.GetEventHash())
	{
		m_CanDefalte = true;
		return;
	}

	if (m_PumpEventHash != event.GetEventHash()) return;

	const auto& pumpEvent = static_cast<const PumpInflatableEvent&>(event);
	m_PumpUser = pumpEvent.GetPumpUser();

	m_CanDefalte = false;
	m_TimeUntilDeflate = m_DeflationDelay;
	m_RenderComp->Enable(false);

	++m_Inflation;

	const auto location = ResourceLocator::GetInstance().GetResource(ResourceLocator::Type::Sound, "pump");
	minigin::ServiceLocator::GetSoundSystem()->Play(location, 1.f);

	if (m_Inflation < m_MaxInflation - 1) return;

	UpdateScoreOfPumper(m_PumpUser, pumpEvent.GetGrid());

	auto hitbox = GetOwner().GetComponent<minigin::Hitbox>();

	if (hitbox != nullptr)
	{
		hitbox->Enable(false);
	}

	auto healthComp = GetOwner().GetComponent<digdug::HealthComponent>();
	if (healthComp != nullptr)
	{
		healthComp->TakeDamage();
		std::cout << "pop" << healthComp->GetHealth() << "\n";
	}

	InflatablePoppedEvent popEvent{ &GetOwner()};
	m_OnPopEvent.Notify(popEvent);
}

void digdug::Inflatable::SetSpriteSheet(const std::string& path)
{
	auto texture = minigin::ResourceManager::GetInstance().LoadTexture(path);

	auto frame = texture->GetSize();
	frame.x /= m_MaxInflation;

	m_SpriteSheet = std::make_unique<minigin::SpriteSheet>(texture, frame);
}

void digdug::Inflatable::UpdateScoreOfPumper(minigin::GameObject* obj, DirtGrid* grid)
{
	if (grid == nullptr) return;
	if (obj == nullptr) return;

	auto scoreComp = obj->GetComponent<digdug::ScoreComponent>();
	if (scoreComp == nullptr) return;

	glm::vec2 size{ 0 };

	auto renderComp = GetOwner().GetComponent<minigin::RenderComponent>();
	if (renderComp != nullptr)
	{
		size = renderComp->GetSize();
	}

	const auto center = GetOwner().GetLocalPosition() + glm::vec3(size / 2.f, 0.f);
	auto gridPos = grid->GetPosInGrid(center);

	const int level = static_cast<int>(grid->GetDepthLevel(gridPos));

	int addedScore = m_BaseScoreValue;

	if (level > 0)
	{
		addedScore += static_cast<int>(addedScore * (level - 1.f) / 2.f);
	}

	scoreComp->AddScore(addedScore);
}
