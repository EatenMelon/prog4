#include "RenderComponent.h"

#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Transform.h"

void minigin::RenderComponent::Render() const
{
	if (!m_Texture) return;

	auto pos = GetOwner().GetWorldPosition();
	Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}

void minigin::RenderComponent::SetTexture(const std::string& filename)
{
	m_Texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void minigin::RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
	if (texture == nullptr) return;

	m_Texture = texture;
}