#pragma once
#include <string>

#include "SDL3/SDL.h"
#include "Component.h"
#include <memory>

namespace minigin
{
	class Texture2D;
	class RenderComponent;
	class Font;
	class TextComponent final : public Component
	{
	public:
		using Component::Component;

		void Start() override;
		void Update(float deltaTime) override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);
		void SetFont(std::shared_ptr<Font> font);

		const std::string& GetText() const;

	private:

		bool m_NeedsUpdate{};
		std::string m_Text{};
		SDL_Color m_Color{ 255, 255, 255, 255 };
		std::shared_ptr<Font> m_font{};

		RenderComponent* m_RenderComponent{ nullptr };
	};
}