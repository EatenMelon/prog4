#include "GameObject.h"

void dae::GameObject::FixedUpdate([[maybe_unused]] float fixedFrameTime)
{
	for (auto& comp : m_Components)
	{
		comp.second->FixedUpdate(fixedFrameTime);
	}
}

void dae::GameObject::Update([[maybe_unused]] float deltaTime)
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
			return c.second->WillBeDestroyed();
		}
	);
}
