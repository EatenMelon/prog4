#pragma once
#include <SDL3/SDL.h>
#include "Singleton.h"
#include <glm/vec2.hpp>

namespace minigin
{
	class Texture2D;
	/**
	 * Simple RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
	public:
		void Init(SDL_Window* window);
		void Render() const;
		void Destroy();

		void RenderTexture(const Texture2D& texture, const glm::vec2& pos) const;
		void RenderTexture(const Texture2D& texture, const glm::vec2& pos, const glm::vec2& size) const;
		void RenderTexture(const Texture2D& texture, const glm::vec2& pos, const glm::vec2& size, float angle) const;
		void RenderTexture(const Texture2D& texture, const glm::vec2& pos, const glm::vec2& size, float angle, SDL_FlipMode flipmode) const;

		SDL_Renderer* GetSDLRenderer() const;

		const SDL_Color& GetBackgroundColor() const { return m_clearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_clearColor = color; }

	private:

		SDL_Renderer* m_renderer{};
		SDL_Window* m_window{};
		SDL_Color m_clearColor{};
	};
}

