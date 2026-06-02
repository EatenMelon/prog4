#include "GameObject.h"
#include "GameObject.h"

void minigin::GameObject::Init()
{
	for (auto& comp : m_Components)
	{
		comp.second->Init();
	}
}

void minigin::GameObject::FixedUpdate(float fixedFrameTime)
{
	for (auto& comp : m_Components)
	{
		comp.second->FixedUpdate(fixedFrameTime);
	}
}

void minigin::GameObject::Update(float deltaTime)
{
	for (auto& comp : m_Components)
	{
		comp.second->Update(deltaTime);
	}
}

void minigin::GameObject::GuiRender() const
{
	for (auto& comp : m_Components)
	{
		comp.second->GuiRender();
	}
}

void minigin::GameObject::Render() const
{
	for (auto& comp : m_Components)
	{
		comp.second->Render();
	}
}

void minigin::GameObject::Cleanup()
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

const glm::vec3& minigin::GameObject::GetLocalPosition()
{
	return m_LocalTransform.GetPosition();
}

void minigin::GameObject::SetLocalPosition(float x, float y, float z)
{
	m_LocalTransform.SetPosition(x, y, z);
	SetPositionDirty();
}

void minigin::GameObject::SetLocalPosition(const glm::vec3& position)
{
	m_LocalTransform.SetPosition(position);
	SetPositionDirty();
}

const glm::vec3& minigin::GameObject::GetWorldPosition()
{
	UpdateWorldPosition();

	return m_WorldTransform.GetPosition();
}

void minigin::GameObject::SetWorldPosition(float x, float y, float z)
{
	m_WorldTransform.SetPosition(x, y, z);
	SetPositionDirty();
}

void minigin::GameObject::SetWorldPosition(const glm::vec3& position)
{
	m_WorldTransform.SetPosition(position);
	SetPositionDirty();
}

void minigin::GameObject::SetPositionDirty()
{
	m_PositionIsDirty = true;

	for (auto child : m_Children)
	{
		child->SetPositionDirty();
	}
}

void minigin::GameObject::UpdateWorldPosition()
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

void minigin::GameObject::Destroy()
{
	m_Destroyed = true;

	for (auto child : m_Children)
	{
		child->Destroy();
	}
}

bool minigin::GameObject::MarkedForDestruction() const
{
	return m_Destroyed;
}

void minigin::GameObject::SetParent(GameObject* parent, bool keepWorldTransform)
{
	if (parent == nullptr)
	{
		SetLocalPosition(GetWorldPosition());
	}

	if (parent == this) return;
	if (parent == m_Parent) return;
	if (IsChild(parent)) return;

	if (keepWorldTransform)
	{
		SetLocalPosition(GetWorldPosition() - parent->GetWorldPosition());
	}
	else
	{
		SetPositionDirty();
	}

	if (m_Parent) m_Parent->RemoveChild(this);

	m_Parent = parent;

	if (m_Parent) m_Parent->AddChild(this);
}

minigin::GameObject* minigin::GameObject::GetParent() const
{
	return m_Parent;
}

size_t minigin::GameObject::GetChildCount() const
{
	return m_Children.size();
}

minigin::GameObject* minigin::GameObject::GetChildAt(size_t index) const
{
	if (index >= m_Children.size()) return nullptr;

	return m_Children[index];
}

bool minigin::GameObject::IsChild(GameObject* parent) const
{
	return std::find(m_Children.begin(), m_Children.end(), parent) != m_Children.end();
}

void minigin::GameObject::AddChild(GameObject* child)
{
	if (IsChild(child)) return;

	m_Children.push_back(child);
}

void minigin::GameObject::RemoveChild(GameObject* child)
{
	if (!IsChild(child)) return;

	std::erase(m_Children, child);
}