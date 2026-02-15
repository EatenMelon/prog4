#include <string>
#include <type_traits>

#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"

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
	const auto& pos = m_transform.GetPosition();
	Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
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

void dae::GameObject::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_transform.SetPosition(x, y, 0.0f);
}
