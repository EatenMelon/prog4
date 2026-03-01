#pragma once
#include <SDL3/SDL.h>
#include "Singleton.h"
#include <vector>
#include <functional>

namespace dae
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

		void RenderTexture(const Texture2D& texture, float x, float y) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height) const;

		void AddImguiRenderFunction(const std::function<void()>& function);
		void ClearImguiRenderFunctions();

		SDL_Renderer* GetSDLRenderer() const;

		const SDL_Color& GetBackgroundColor() const { return m_clearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_clearColor = color; }

	private:
		void ExecuteImGuiRenderFunctions() const;

		SDL_Renderer* m_renderer{};
		SDL_Window* m_window{};
		SDL_Color m_clearColor{};

		std::vector<std::function<void()>> m_ImGuiRenderFunctions{};
	};
}

