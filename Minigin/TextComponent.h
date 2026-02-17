#pragma once
#include <string>

#include "Component.h"
#include "SDL3/SDL.h"

namespace dae
{
	class Texture2D;
	class Font;
	class TextComponent : public Component
	{
	public:
		using Component::Component;

		void Update(float deltaTime) override;
		void Render() const override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);
		void SetFont(std::shared_ptr<Font> font);

	private:

		bool m_needsUpdate{};
		std::string m_text{};
		SDL_Color m_color{ 255, 255, 255, 255 };
		std::shared_ptr<Font> m_font{};
		std::shared_ptr<Texture2D> m_textTexture{};
	};
}