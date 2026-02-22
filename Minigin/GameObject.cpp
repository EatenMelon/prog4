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

const glm::vec3& dae::GameObject::GetLocalPosition()
{
	return m_LocalTransform.GetPosition();
}

void dae::GameObject::SetLocalPosition(float x, float y, float z)
{
	m_LocalTransform.SetPosition(x, y, z);
}

void dae::GameObject::SetLocalPosition(const glm::vec3& position)
{
	m_LocalTransform.SetPosition(position);
	SetPositionDirty();
}

const glm::vec3& dae::GameObject::GetWorldPosition()
{
	UpdateWorldPosition();

	return m_WorldTransform.GetPosition();
}

void dae::GameObject::SetWorldPosition(float x, float y, float z)
{
	m_WorldTransform.SetPosition(x, y, z);
}

void dae::GameObject::SetWorldPosition(const glm::vec3& position)
{
	m_WorldTransform.SetPosition(position);
}

void dae::GameObject::SetPositionDirty()
{
	m_PositionIsDirty = true;
}

void dae::GameObject::UpdateWorldPosition()
{
	if (!m_PositionIsDirty) return;

	if (m_Parent == nullptr)
	{
		m_WorldTransform.SetPosition(m_LocalTransform.GetPosition());
	}
	else
	{
		m_WorldTransform.SetPosition(m_Parent->GetWorldPosition() + m_LocalTransform.GetPosition());
	}

	m_PositionIsDirty = false;
}

void dae::GameObject::Destroy()
{
	m_Destroyed = true;
}

bool dae::GameObject::MarkedForDestruction() const
{
	return m_Destroyed;
}

void dae::GameObject::SetParent(GameObject* parent, bool keepWorldTransform)
{
	if (parent == nullptr)
	{
		SetLocalPosition(GetWorldPosition());
		m_Parent = parent;
		return;
	}

	if (parent == this) return;
	if (parent == m_Parent) return;
	if (IsChild(parent)) return;

	if (keepWorldTransform)
	{
		SetLocalPosition(GetWorldPosition() - parent->GetWorldPosition());
		SetPositionDirty();
	}

	m_Parent->RemoveChild(this);

	m_Parent = parent;

	m_Parent->AddChild(this);
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

	return m_Children[index];
}

bool dae::GameObject::IsChild(GameObject* parent) const
{
	return std::find(m_Children.begin(), m_Children.end(), parent) != m_Children.end();;
}

void dae::GameObject::AddChild(GameObject* child)
{
	if (IsChild(child)) return;

	m_Children.push_back(child);
}

void dae::GameObject::RemoveChild(GameObject* child)
{
	std::erase(m_Children, child);
}

