#include <string>
#include <type_traits>

#include "GameObject.h"

dae::GameObject::~GameObject() = default;

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

void dae::GameObject::SetTexture([[maybe_unused]] const std::string& filename)
{
	//m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::GameObject::SetPosition([[maybe_unused]] float x, [[maybe_unused]] float y)
{
	//m_transform.SetPosition(x, y, 0.0f);
}
