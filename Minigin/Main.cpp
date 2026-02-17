#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"

#include "Component.h"
#include "FpsCounterComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);

	// adding the background image to the scene
	auto bg = std::make_shared<dae::GameObject>();
	bg->AddComponent<dae::Transform>();
	auto bgTexture = bg->AddComponent<dae::RenderComponent>();

	// set the texture when a texture has been added
	if (bgTexture != nullptr) bgTexture->SetTexture("background.png");

	scene.Add(std::move(bg));

	// adding the title text to the scene

	auto title = std::make_shared<dae::GameObject>();
	auto titlePos = title->AddComponent<dae::Transform>();

	// set the position when one has been added
	if (titlePos != nullptr) titlePos->SetPosition(292.f, 20.f);

	auto titleText = title->AddComponent<dae::TextComponent>();

	if (titleText != nullptr)
	{
		titleText->SetFont(font);
		titleText->SetColor({ 255, 255, 0 });
		titleText->SetText("Programming 4 Assignment");
	}
	scene.Add(std::move(title));
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
