#include "GameObject.h"

void dae::GameObject::FixedUpdate(float fixedFrameTime)
{
	for (auto& comp : m_Components)
	{
		comp.second->FixedUpdate(fixedFrameTime);
	}
}

void dae::GameObject::Update(float deltaTime)
{
	for (auto& comp : m_Components)
	{
		comp.second->Update(deltaTime);
	}
}

void dae::GameObject::Render() const
{
	for (auto& comp : m_Components)
	{
		comp.second->Render();
	}
}

void dae::GameObject::Cleanup()
{
	std::erase_if
	(
		m_Components,
		[](const auto& c)
		{
			return c.second->MarkedForDestruction();
		}
	);
}

const glm::vec3& dae::GameObject::GetPosition()
{
	return m_Transform.GetPosition();
}

void dae::GameObject::SetPosition(float x, float y, float z)
{
	m_Transform.SetPosition(x, y, z);
}

void dae::GameObject::Destroy()
{
	m_Destroyed = true;
}

bool dae::GameObject::MarkedForDestruction() const
{
	return m_Destroyed;
}
