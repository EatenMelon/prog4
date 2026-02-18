#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"

#include "FpsCounterComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "Transform.h"

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);

	// adding the background image to the scene
	{
		auto background = std::make_unique<dae::GameObject>();
		background->AddComponent<dae::Transform>();
		auto bgTexture = background->AddComponent<dae::RenderComponent>();

		// set the texture when a texture has been added
		if (bgTexture != nullptr) bgTexture->SetTexture("background.png");

		scene.Add(std::move(background));
	}

	// adding the logo image to the scene
	{
		auto logo = std::make_unique<dae::GameObject>();
		auto pos = logo->AddComponent<dae::Transform>();

		// set the position when one has been added
		if (pos != nullptr) pos->SetPosition(358.f, 180.f);

		auto bgTexture = logo->AddComponent<dae::RenderComponent>();

		// set the texture when a texture has been added
		if (bgTexture != nullptr) bgTexture->SetTexture("logo.png");

		scene.Add(std::move(logo));
	}

	// adding the title text to the scene
	{
		auto title = std::make_unique<dae::GameObject>();
		auto pos = title->AddComponent<dae::Transform>();

		// set the position when one has been added
		if (pos != nullptr) pos->SetPosition(292.f, 20.f);

		auto text = title->AddComponent<dae::TextComponent>();

		if (text != nullptr)
		{
			text->SetFont(font);
			text->SetColor({ 255, 255, 0 });
			text->SetText("Programming 4 Assignment");
		}
		scene.Add(std::move(title));
	}

	// adding the fps counter to the screen
	{
		auto fpsCounter = std::make_unique<dae::GameObject>();
		auto pos = fpsCounter->AddComponent<dae::Transform>();

		// set the position when one has been added
		if (pos != nullptr) pos->SetPosition(20.f, 20.f);

		auto text = fpsCounter->AddComponent<dae::FpsCounterComponent>();

		if (text != nullptr)
		{
			text->SetFont(font);
			text->SetColor({ 255, 255, 0 });
		}
		scene.Add(std::move(fpsCounter));
	}
}

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);
    return 0;
}
