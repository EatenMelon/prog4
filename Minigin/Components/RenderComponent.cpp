#include "RenderComponent.h"
#include "RenderComponent.h"
#include "RenderComponent.h"
#include "RenderComponent.h"
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

	Renderer::GetInstance().RenderTexture(*m_Texture, pos, size, m_Rotation, GetSDLFlipMode());
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

void minigin::RenderComponent::MatchWidth(float width)
{
	if (m_Texture == nullptr) return;

	const float original = m_Texture->GetSize().x;

	m_UniformScale = width / original;
}

void minigin::RenderComponent::MatchHeight(float height)
{
	if (m_Texture == nullptr) return;

	const float original = m_Texture->GetSize().y;

	m_UniformScale = height / original;
}

glm::vec2 minigin::RenderComponent::GetSize() const
{
	if (!m_Texture) return glm::vec2(0);

	return m_Texture->GetSize() * m_UniformScale;
}

float minigin::RenderComponent::GetUniformScale() const
{
	return m_UniformScale;
}

float minigin::RenderComponent::GetRotation() const
{
	return m_Rotation;
}

SDL_FlipMode minigin::RenderComponent::GetSDLFlipMode() const
{
	if (m_HorFlip && m_VerFlip)
	{
		return SDL_FLIP_HORIZONTAL_AND_VERTICAL;
	}
	else if (m_HorFlip)
	{
		return SDL_FLIP_HORIZONTAL;
	}
	else if (m_VerFlip)
	{
		return SDL_FLIP_VERTICAL;
	}

	return SDL_FLIP_NONE;
}
