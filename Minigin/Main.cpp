#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Renderer.h"

#include "FpsCounterComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"

#include <filesystem>
namespace fs = std::filesystem;

#include"CacheExercises.h"

static void load()
{
	dae::Renderer::GetInstance().AddImguiRenderFunction(dae::ImGuiTestExercise1);
	dae::Renderer::GetInstance().AddImguiRenderFunction(dae::ImGuiTestExercise2);

	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	
	auto background = std::make_unique<dae::GameObject>();
	{
		auto renderComponent = background->AddComponent<dae::RenderComponent>();
		if (renderComponent != nullptr) renderComponent->SetTexture("background.png");

		scene.Add(std::move(background));
	}

	auto logo = std::make_unique<dae::GameObject>();
	{
		logo->SetLocalPosition(358.f, 180.f);

		auto renderComp = logo->AddComponent<dae::RenderComponent>();
		if (renderComp != nullptr) renderComp->SetTexture("logo.png");

		scene.Add(std::move(logo));
	}

	
	auto title = std::make_unique<dae::GameObject>();
	{
		title->SetLocalPosition(292.f, 20.f);

		auto renderComp = title->AddComponent<dae::RenderComponent>();

		if(renderComp != nullptr)
		{
			auto text = title->AddComponent<dae::TextComponent>();

			if (text != nullptr)
			{
				text->SetFont(font);
				text->SetColor({ 255, 255, 0, 255 });
				text->SetText("Programming 4 Assignment");
			}
		}
		scene.Add(std::move(title));
	}

	
	auto fpsCounter = std::make_unique<dae::GameObject>();
	{
		fpsCounter->SetLocalPosition(20.f, 20.f);

		auto renderComp = fpsCounter->AddComponent<dae::RenderComponent>();

		if (renderComp != nullptr)
		{
			auto text = fpsCounter->AddComponent<dae::TextComponent>();

			if (text != nullptr)
			{
				text->SetText("...");
				text->SetFont(font);
				text->SetColor({ 255, 255, 0, 255 });

				fpsCounter->AddComponent<dae::FpsCounterComponent>();
			}
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