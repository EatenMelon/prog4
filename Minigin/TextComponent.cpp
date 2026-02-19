#include "TextComponent.h"

#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>

#include "GameObject.h"
#include "Texture2D.h"
#include "RenderComponent.h"
#include "Renderer.h"
#include "Font.h"

void dae::TextComponent::Init()
{
	m_RenderComponent = GetOwner().GetComponent<RenderComponent>();
	m_NeedsUpdate = true;
}

void dae::TextComponent::Update([[maybe_unused]] float deltaTime)
{
	if (!m_RenderComponent) return;

	if (m_NeedsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_font->GetFont(), m_Text.c_str(), m_Text.length(), m_Color);
		if (surf == nullptr)
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}
		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
		if (texture == nullptr)
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}
		SDL_DestroySurface(surf);
		m_RenderComponent->SetTexture(std::make_shared<Texture2D>(texture));
		m_NeedsUpdate = false;
	}
}

void dae::TextComponent::SetText(const std::string& text)
{
	if (m_Text == text) return;

	m_Text = text;
	m_NeedsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_Color = color;
	m_NeedsUpdate = true;
}

void dae::TextComponent::SetFont(std::shared_ptr<Font> font)
{
	m_font = std::move(font);
}
