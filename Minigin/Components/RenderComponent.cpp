#include "RenderComponent.h"

#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include <Texture2D.h>

#include <algorithm>
#include <limits>

void minigin::RenderComponent::Render() const
{
	if (!m_Texture) return;

	auto pos = GetOwner().GetWorldPosition();
	auto size = m_Texture->GetSize() * m_UniformScale;

	SDL_FlipMode flipMode{ SDL_FLIP_NONE };

	if (m_HorFlip && m_VerFlip)
	{
		flipMode = SDL_FLIP_HORIZONTAL_AND_VERTICAL;
	}
	else if (m_HorFlip)
	{
		flipMode = SDL_FLIP_HORIZONTAL;
	}
	else if (m_VerFlip)
	{
		flipMode = SDL_FLIP_VERTICAL;
	}

	Renderer::GetInstance().RenderTexture(*m_Texture, pos, size, m_Rotation, flipMode);
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

void minigin::RenderComponent::SetUniformScale(float scale)
{
	m_UniformScale = std::max({ scale, std::numeric_limits<float>::min()});
}

glm::vec2 minigin::RenderComponent::GetSize() const
{
	if (!m_Texture) return glm::vec2(0);

	return m_Texture->GetSize() * m_UniformScale;
}
