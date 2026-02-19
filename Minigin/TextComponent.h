#pragma once
#include <string>

#include "SDL3/SDL.h"
#include "Component.h"

namespace dae
{
	class Texture2D;
	class Font;
	class TextComponent final : public Component
	{
	public:
		using Component::Component;

		void Init() override;
		void Update(float deltaTime) override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);
		void SetFont(std::shared_ptr<Font> font);

	private:

		bool m_needsUpdate{};
		std::string m_text{};
		SDL_Color m_color{ 255, 255, 255, 255 };
		std::shared_ptr<Font> m_font{};
	};
}