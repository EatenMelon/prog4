#include "RenderComponent.h"

#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Transform.h"

void dae::RenderComponent::Render() const
{
	const auto& transf = GetOwner().GetComponent<dae::Transform>();

	if (!transf) return;

	auto pos = transf->GetPosition();

	Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}

void dae::RenderComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}
