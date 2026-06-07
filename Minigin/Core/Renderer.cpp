#include "Renderer.h"
#include "Renderer.h"
#include <stdexcept>
#include <iostream>
#include "SceneManager.h"
#include "Texture2D.h"
#include "SpriteSheet.h"

// imgui
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

void minigin::Renderer::Init(SDL_Window* window)
{
	m_window = window;
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	m_renderer = SDL_CreateRenderer(window, nullptr);
	if (m_renderer == nullptr)
	{
		std::cout << "Failed to create the renderer: " << SDL_GetError() << "\n";
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

#if defined(__EMSCRIPTEN__)
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = NULL;
#endif

	ImGui_ImplSDL3_InitForSDLRenderer(window, m_renderer);
	ImGui_ImplSDLRenderer3_Init(m_renderer);
}

void minigin::Renderer::Render() const
{
	// imgui
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	SceneManager::GetInstance().GuiRender();

	ImGui::Render();
	ImGui::EndFrame();

	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer);

	SceneManager::GetInstance().Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);
	SDL_RenderPresent(m_renderer);
}

void minigin::Renderer::Destroy()
{
	// imgui cleanup
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
}

void minigin::Renderer::RenderTexture
(
	const Texture2D& texture,
	const glm::vec2& pos,
	float angle,
	SDL_FlipMode flipmode
) const
{
	SDL_FRect dst{};
	dst.x = pos.x;
	dst.y = pos.y;
	SDL_GetTextureSize(texture.GetSDLTexture(), &dst.w, &dst.h);
	SDL_RenderTextureRotated(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst, angle, nullptr, flipmode);
}

void minigin::Renderer::RenderTexture
(
	const Texture2D& texture,
	const glm::vec2& pos,
	const glm::vec2& size,
	float angle,
	SDL_FlipMode flipmode
) const
{
	SDL_FRect dst{};
	dst.x = pos.x;
	dst.y = pos.y;
	dst.w = size.x;
	dst.h = size.y;

	SDL_FPoint center{};
	center.x = size.x / 2;
	center.y = size.y / 2;

	SDL_RenderTextureRotated(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst, angle, &center, flipmode);
}

void minigin::Renderer::RenderSprite
(
	const SpriteSheet& spriteSheet,
	const glm::vec2& pos,
	const glm::ivec2& spritePos,
	float scale,
	float angle,
	SDL_FlipMode flipmode
) const
{
	auto src = spriteSheet.GetSpriteCutout(spritePos);

	SDL_FRect dst{};
	dst.x = pos.x;
	dst.y = pos.y;
	dst.w = spriteSheet.GetSpriteSize().x * scale;
	dst.h = spriteSheet.GetSpriteSize().y * scale;

	SDL_FPoint center{};
	center.x = dst.w / 2;
	center.y = dst.h / 2;

	SDL_RenderTextureRotated(GetSDLRenderer(), spriteSheet.GetSDLTexture(), &src, &dst, angle, &center, flipmode);
}

SDL_Renderer* minigin::Renderer::GetSDLRenderer() const { return m_renderer; }
