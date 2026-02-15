#include "Component.h"
#include "GameObject.h"

dae::Component::Component(std::weak_ptr<GameObject> owner)
	: m_Owner{ owner }
{

}

void dae::Component::FixedUpdate([[maybe_unused]] float fixedFrameTime)
{
	// empty, because i want to keep it optional
}

void dae::Component::Update([[maybe_unused]] float deltaTime)
{
	// empty, because i want to keep it optional
}

dae::GameObject& dae::Component::GetOwner() const
{
	return *m_Owner.lock().get();
}
