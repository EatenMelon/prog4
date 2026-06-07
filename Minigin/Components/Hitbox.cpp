#include "Hitbox.h"
#include <GameObject.h>
#include <Subject.h>
#include <Events.h>
#include <HitboxManager.h>
#include <Renderer.h>

minigin::Hitbox::~Hitbox()
{
	HitboxManager::GetInstance().Leave(*this);
}

void minigin::Hitbox::Init()
{
	HitboxManager::GetInstance().Join(*this);
}

void minigin::Hitbox::Render() const
{
	auto renderer = Renderer::GetInstance().GetSDLRenderer();

	const auto shrink = glm::vec2(m_Shrink, m_Shrink);

	SDL_FRect rect{};
	rect.x = GetOwner().GetWorldPosition().x + m_Shrink / 2;
	rect.y = GetOwner().GetWorldPosition().y + m_Shrink / 2;
	rect.w = m_Width - shrink.x;
	rect.h = m_Height - shrink.y;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderRect(renderer, &rect);
}

void minigin::Hitbox::SetBounds(float width, float height)
{
	m_Width = width;
	m_Height = height;
}

bool minigin::Hitbox::IsColliding(const Hitbox& other) const
{
	if (&other == this) return false;
	if (&GetOwner() == &other.GetOwner()) return false;

	const auto shrink = glm::vec3(m_Shrink, m_Shrink, 0.f);

	const auto myPos = GetOwner().GetWorldPosition() + shrink / glm::vec3(2);
	const auto mySize = glm::vec3(m_Width, m_Height, 0.f) - shrink;

	const auto otherPos = other.GetOwner().GetWorldPosition() + shrink / glm::vec3(2);
	const auto otherSize = glm::vec3(other.m_Width, other.m_Height, 0.f) - shrink;

	if (myPos.x > otherPos.x + otherSize.x) return false;
	if (myPos.x + mySize.x < otherPos.x) return false;

	if (myPos.y > otherPos.y + otherSize.y) return false;
	if (myPos.y + mySize.y < otherPos.y) return false;

	return true;
}

void minigin::Hitbox::CheckCollision(Hitbox& other)
{
	if (!Enabled()) return;

	bool isColliding = IsColliding(other);

	auto itr = std::find
	(
		m_CurrentlyColliding.begin(),
		m_CurrentlyColliding.end(),
		&other
	);

	bool wasColliding = itr != m_CurrentlyColliding.end();

	if (isColliding && wasColliding)
	{
		HitEvent event{ &other };

		if (!other.GetOwner().Enabled() || !other.Enabled())
		{
			m_CurrentlyColliding.erase(itr);
			m_OnHitExit.Notify(event);
			return;
		}

		m_OnHitStay.Notify(event);
	}
	else if (isColliding && !wasColliding)
	{
		if (!other.GetOwner().Enabled() || !other.Enabled())
		{
			return;
		}

		m_CurrentlyColliding.push_back(&other);
		
		HitEvent event{ &other };
		m_OnHitEnter.Notify(event);
	}
	else if (!isColliding && wasColliding)
	{
		m_CurrentlyColliding.erase(itr);
		
		HitEvent event{ &other };
		m_OnHitExit.Notify(event);
	}
}
