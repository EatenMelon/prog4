#include "Component.h"
#include "GameObject.h"

dae::Component::Component(GameObject* owner)
	: m_Owner{ owner }
	, m_Destroyed{ false }
{

}

void dae::Component::Init() { }

void dae::Component::FixedUpdate(float) {  }

void dae::Component::Update(float) {  }

void dae::Component::Render() const {  }

void dae::Component::Destroy()
{
	m_Destroyed = true;
}

bool dae::Component::MarkedForDestruction() const
{
	return m_Destroyed;
}

dae::GameObject& dae::Component::GetOwner() const
{
	return *m_Owner;
}

