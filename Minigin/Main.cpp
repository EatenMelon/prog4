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

#include "InputManager.h"
#include "Command.h"

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto smallFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 18);
	
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

		if (renderComp != nullptr)
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

	auto contolsTaizoHori = std::make_unique<dae::GameObject>();
	{
		contolsTaizoHori->SetLocalPosition(15.f, 80.f);

		auto renderComp = contolsTaizoHori->AddComponent<dae::RenderComponent>();

		if (renderComp != nullptr)
		{
			auto text = contolsTaizoHori->AddComponent<dae::TextComponent>();

			if (text != nullptr)
			{
				text->SetFont(smallFont);
				text->SetColor({ 255, 255, 0, 255 });
				text->SetText("Use WASD to move Taizo Hori");
			}
		}
		scene.Add(std::move(contolsTaizoHori));
	}

	auto contolsPooka = std::make_unique<dae::GameObject>();
	{
		contolsPooka->SetLocalPosition(15.f, 110.f);

		auto renderComp = contolsPooka->AddComponent<dae::RenderComponent>();

		if (renderComp != nullptr)
		{
			auto text = contolsPooka->AddComponent<dae::TextComponent>();

			if (text != nullptr)
			{
				text->SetFont(smallFont);
				text->SetColor({ 255, 255, 0, 255 });
				text->SetText("Use the D-Pad to move Pooka");
			}
		}
		scene.Add(std::move(contolsPooka));
	}

	auto TaizoHori = std::make_unique<dae::GameObject>();
	{
		TaizoHori->SetLocalPosition(300.f, 200.f);

		auto moveCommand = std::make_shared<dae::MoveCommand>(TaizoHori.get(), 500.f);

		dae::InputManager::GetInstance().BindInput("Move", SDLK_W, dae::KeyState::Pressed, moveCommand, dae::Direction::Up);
		dae::InputManager::GetInstance().BindInput("Move", SDLK_A, dae::KeyState::Pressed, moveCommand, dae::Direction::Left);
		dae::InputManager::GetInstance().BindInput("Move", SDLK_S, dae::KeyState::Pressed, moveCommand, dae::Direction::Down);
		dae::InputManager::GetInstance().BindInput("Move", SDLK_D, dae::KeyState::Pressed, moveCommand, dae::Direction::Right);

		auto renderComp = TaizoHori->AddComponent<dae::RenderComponent>();
		if (renderComp != nullptr) renderComp->SetTexture("TaizoHori.png");

		scene.Add(std::move(TaizoHori));
	}

	auto Pooka = std::make_unique<dae::GameObject>();
	{
		Pooka->SetLocalPosition(200.f, 200.f);

		auto moveCommand = std::make_shared<dae::MoveCommand>(Pooka.get(), 250.f);

		dae::InputManager::GetInstance().BindInput("Move", dae::GamepadButton::DPAD_UP, dae::KeyState::Pressed, moveCommand, dae::Direction::Up);
		dae::InputManager::GetInstance().BindInput("Move", dae::GamepadButton::DPAD_LEFT, dae::KeyState::Pressed, moveCommand, dae::Direction::Left);
		dae::InputManager::GetInstance().BindInput("Move", dae::GamepadButton::DPAD_DOWN, dae::KeyState::Pressed, moveCommand, dae::Direction::Down);
		dae::InputManager::GetInstance().BindInput("Move", dae::GamepadButton::DPAD_RIGHT, dae::KeyState::Pressed, moveCommand, dae::Direction::Right);

		auto renderComp = Pooka->AddComponent<dae::RenderComponent>();
		if (renderComp != nullptr) renderComp->SetTexture("Pooka.png");

		scene.Add(std::move(Pooka));
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