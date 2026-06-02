#include "Component.h"
#include "GameObject.h"

minigin::Component::Component(GameObject* owner)
	: m_Owner{ owner }
	, m_Destroyed{ false }
{
}

void minigin::Component::Start() { }

void minigin::Component::FixedUpdate(float) { }

void minigin::Component::Update(float) { }

void minigin::Component::GuiRender() const { }

void minigin::Component::Render() const { }

void minigin::Component::Destroy()
{
	m_Destroyed = true;
}

bool minigin::Component::MarkedForDestruction() const
{
	return m_Destroyed;
}

minigin::GameObject& minigin::Component::GetOwner() const
{
	return *m_Owner;
}

