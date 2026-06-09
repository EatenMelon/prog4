#include "Inflatable.h"
#include <ResourceManager.h>
#include <Renderer.h>
#include <RenderComponent.h>
#include <GameObject.h>
#include <HarpoonState.h>
#include <Hitbox.h>

void digdug::Inflatable::Start()
{
	m_RenderComp = GetOwner().GetComponent<minigin::RenderComponent>();

	if (m_SpriteSheet == nullptr)
	{
		std::cerr << "WARNING: InflatableComponent doesn't have a sprite sheet!\n";
	}

	PumpInflatableEvent pumpEvent{ nullptr };
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

	if (m_Inflation < m_MaxInflation - 1) return;

	InflatablePoppedEvent popEvent{};
	m_OnPopEvent.Notify(popEvent);

	auto hitbox = GetOwner().GetComponent<minigin::Hitbox>();
	
	if (hitbox != nullptr)
	{
		hitbox->Enable(false);
	}
}

void digdug::Inflatable::SetSpriteSheet(const std::string& path)
{
	auto texture = minigin::ResourceManager::GetInstance().LoadTexture(path);

	auto frame = texture->GetSize();
	frame.x /= m_MaxInflation;

	m_SpriteSheet = std::make_unique<minigin::SpriteSheet>(texture, frame);
}
