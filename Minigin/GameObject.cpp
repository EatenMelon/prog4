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

void dae::GameObject::SetParent(const std::unique_ptr<GameObject>& parent, bool keepWorldTransform)
{
	// not null, not this gameobject, not current parent
	if (parent == nullptr) return;
	if (parent.get() == this) return;
	if (parent.get() == m_Parent) return;

	if (m_Parent != nullptr)
	{
		bool isNewChild = std::find(m_Children.begin(), m_Children.end(), this) == m_Children.end();

		if (isNewChild) m_Children.push_back(this);
	}

	m_Parent = parent.get();

	// update transform
}

dae::GameObject* dae::GameObject::GetParent() const
{
	return m_Parent;
}

size_t dae::GameObject::GetChildCount() const
{
	return m_Children.size();
}

dae::GameObject* dae::GameObject::GetChildAt(size_t index) const
{
	if (index >= m_Children.size()) return nullptr;
}

