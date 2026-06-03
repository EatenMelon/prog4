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

void minigin::Hitbox::Start()
{
	HitboxManager::GetInstance().Join(*this);
}

void minigin::Hitbox::Render() const
{
	auto renderer = Renderer::GetInstance().GetSDLRenderer();

	SDL_FRect rect{};
	rect.x = GetOwner().GetWorldPosition().x;
	rect.y = GetOwner().GetWorldPosition().y;
	rect.w = m_Width;
	rect.h = m_Height;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderRect(renderer, &rect);
}

void minigin::Hitbox::SetSize(float width, float height)
{
	m_Width = width;
	m_Height = height;
}

bool minigin::Hitbox::IsColliding(const Hitbox& other) const
{
	if (&other == this) return false;
	if (&GetOwner() == &other.GetOwner()) return false;

	auto myPos = GetOwner().GetWorldPosition();
	auto otherPos = other.GetOwner().GetWorldPosition();

	if (myPos.x > otherPos.x + other.m_Width) return false;
	if (myPos.x + m_Width < otherPos.x) return false;

	if (myPos.y > otherPos.y + other.m_Height) return false;
	if (myPos.y + m_Height < otherPos.y) return false;

	return true;
}

void minigin::Hitbox::CheckCollision(Hitbox& other)
{
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
		m_OnHitStay.Notify(event);
	}
	else if (isColliding && !wasColliding)
	{
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
