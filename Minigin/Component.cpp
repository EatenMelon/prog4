#include "Component.h"
#include "GameObject.h"

dae::Component::Component(std::weak_ptr<GameObject> owner)
	: m_Owner{ owner }
	, m_Destroyed{ false }
{

}

void dae::Component::FixedUpdate([[maybe_unused]] float fixedFrameTime) { }

void dae::Component::Update([[maybe_unused]] float deltaTime) { }

void dae::Component::Destroy()
{
	m_Destroyed = true;
}

bool dae::Component::WillBeDestroyed() const
{
	return m_Destroyed;
}

dae::GameObject& dae::Component::GetOwner() const
{
	return *m_Owner.lock().get();
}

