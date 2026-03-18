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

#include "HealthComponent.h"
#include "HealthDisplay.h"
#include "ScoreComponent.h"
#include "ScoreDisplay.h"
#include "Achievement.h"

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

	auto controlsTaizoHori = std::make_unique<dae::GameObject>();
	{
		controlsTaizoHori->SetLocalPosition(15.f, 80.f);

		auto renderComp = controlsTaizoHori->AddComponent<dae::RenderComponent>();

		if (renderComp != nullptr)
		{
			auto text = controlsTaizoHori->AddComponent<dae::TextComponent>();

			if (text != nullptr)
			{
				text->SetFont(smallFont);
				text->SetColor({ 255, 255, 0, 255 });
				text->SetText("Use WASD to move Taizo Hori, C to inflict damage, and X to Pick up carrots.");
			}
		}
		scene.Add(std::move(controlsTaizoHori));
	}

	auto controlsPooka = std::make_unique<dae::GameObject>();
	{
		controlsPooka->SetLocalPosition(15.f, 110.f);

		auto renderComp = controlsPooka->AddComponent<dae::RenderComponent>();

		if (renderComp != nullptr)
		{
			auto text = controlsPooka->AddComponent<dae::TextComponent>();

			if (text != nullptr)
			{
				text->SetFont(smallFont);
				text->SetColor({ 255, 255, 0, 255 });
				text->SetText("Use the D-Pad to move Pooka, X to inflict damage, and A to Pick up carrots.");
			}
		}
		scene.Add(std::move(controlsPooka));
	}

	auto TaizoHori = std::make_unique<dae::GameObject>();
	{
		auto HealthTaizoHori = std::make_unique<dae::GameObject>();
		auto healthDisplayTaizoHori = HealthTaizoHori->AddComponent<dae::HealthDisplay>();
		{
			HealthTaizoHori->SetLocalPosition(15.f, 150.f);

			auto renderComp = HealthTaizoHori->AddComponent<dae::RenderComponent>();

			if (renderComp != nullptr)
			{
				auto text = HealthTaizoHori->AddComponent<dae::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");
				}
			}
		}

		auto ScoreTaizoHori = std::make_unique<dae::GameObject>();
		auto scoreDisplayTaizoHori = ScoreTaizoHori->AddComponent<dae::ScoreDisplay>();
		{
			ScoreTaizoHori->SetLocalPosition(15.f, 170.f);

			auto renderComp = ScoreTaizoHori->AddComponent<dae::RenderComponent>();

			if (renderComp != nullptr)
			{
				auto text = ScoreTaizoHori->AddComponent<dae::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");
				}
			}
		}

		TaizoHori->SetLocalPosition(300.f, 200.f);

		auto moveCommand = std::make_shared<dae::MoveCommand>(TaizoHori.get(), 500.f);

		dae::InputManager::GetInstance().BindInput("Move", SDLK_W, dae::KeyState::Pressed, moveCommand, dae::Direction::Up);
		dae::InputManager::GetInstance().BindInput("Move", SDLK_A, dae::KeyState::Pressed, moveCommand, dae::Direction::Left);
		dae::InputManager::GetInstance().BindInput("Move", SDLK_S, dae::KeyState::Pressed, moveCommand, dae::Direction::Down);
		dae::InputManager::GetInstance().BindInput("Move", SDLK_D, dae::KeyState::Pressed, moveCommand, dae::Direction::Right);

		auto renderComp = TaizoHori->AddComponent<dae::RenderComponent>();
		if (renderComp != nullptr) renderComp->SetTexture("TaizoHori.png");

		auto health = TaizoHori->AddComponent<dae::HealthComponent>();
		if (health != nullptr && healthDisplayTaizoHori != nullptr)
		{
			health->Subscrube(healthDisplayTaizoHori);
			healthDisplayTaizoHori->SetHealthComponent(health);
		}

		auto damageCommand = std::make_shared<dae::DamageCommand>(TaizoHori.get(), health);
		dae::InputManager::GetInstance().BindInput("Damage", SDLK_C, dae::KeyState::OnDown, damageCommand);

		auto score = TaizoHori->AddComponent<dae::ScoreComponent>();
		if (score != nullptr && scoreDisplayTaizoHori != nullptr)
		{
			score->Subscrube(scoreDisplayTaizoHori);
			scoreDisplayTaizoHori->SetScoreComponent(score);

			// steam achievement
			auto Achievement = ScoreTaizoHori->AddComponent<dae::Achievement>();
			if (Achievement != nullptr)
			{
				score->Subscrube(Achievement);
				Achievement->SetScoreComponent(score);
			}

		}

		auto scoreCommand = std::make_shared<dae::ScoreCommand>(TaizoHori.get(), score);
		dae::InputManager::GetInstance().BindInput("Score", SDLK_X, dae::KeyState::OnDown, scoreCommand);

		scene.Add(std::move(TaizoHori));
		scene.Add(std::move(HealthTaizoHori));
		scene.Add(std::move(ScoreTaizoHori));
	}

	auto Pooka = std::make_unique<dae::GameObject>();
	{
		auto HealthPooka = std::make_unique<dae::GameObject>();
		auto healthDisplayPooka = HealthPooka->AddComponent<dae::HealthDisplay>();
		{
			HealthPooka->SetLocalPosition(15.f, 210.f);

			auto renderComp = HealthPooka->AddComponent<dae::RenderComponent>();

			if (renderComp != nullptr)
			{
				auto text = HealthPooka->AddComponent<dae::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");
				}
			}
		}

		auto ScorePooka = std::make_unique<dae::GameObject>();
		auto scoreDisplayPooka = ScorePooka->AddComponent<dae::ScoreDisplay>();
		{
			ScorePooka->SetLocalPosition(15.f, 230.f);

			auto renderComp = ScorePooka->AddComponent<dae::RenderComponent>();

			if (renderComp != nullptr)
			{
				auto text = ScorePooka->AddComponent<dae::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");
				}
			}
		}

		Pooka->SetLocalPosition(200.f, 200.f);

		auto moveCommand = std::make_shared<dae::MoveCommand>(Pooka.get(), 250.f);

		dae::InputManager::GetInstance().BindInput("Move", dae::GamepadButton::DPAD_UP, dae::KeyState::Pressed, moveCommand, dae::Direction::Up);
		dae::InputManager::GetInstance().BindInput("Move", dae::GamepadButton::DPAD_LEFT, dae::KeyState::Pressed, moveCommand, dae::Direction::Left);
		dae::InputManager::GetInstance().BindInput("Move", dae::GamepadButton::DPAD_DOWN, dae::KeyState::Pressed, moveCommand, dae::Direction::Down);
		dae::InputManager::GetInstance().BindInput("Move", dae::GamepadButton::DPAD_RIGHT, dae::KeyState::Pressed, moveCommand, dae::Direction::Right);

		auto renderComp = Pooka->AddComponent<dae::RenderComponent>();
		if (renderComp != nullptr) renderComp->SetTexture("Pooka.png");

		auto health = Pooka->AddComponent<dae::HealthComponent>();
		if (health != nullptr)
		{
			health->Subscrube(healthDisplayPooka);
			healthDisplayPooka->SetHealthComponent(health);
		}

		auto damageCommand = std::make_shared<dae::DamageCommand>(Pooka.get(), health);
		dae::InputManager::GetInstance().BindInput("Damage", dae::GamepadButton::WEST, dae::KeyState::OnDown, damageCommand);

		auto score = Pooka->AddComponent<dae::ScoreComponent>();
		if (score != nullptr)
		{
			score->Subscrube(scoreDisplayPooka);
			scoreDisplayPooka->SetScoreComponent(score);
		}

		auto scoreCommand = std::make_shared<dae::ScoreCommand>(Pooka.get(), score);
		dae::InputManager::GetInstance().BindInput("Score", dae::GamepadButton::SOUTH, dae::KeyState::OnDown, scoreCommand);

		scene.Add(std::move(Pooka));
		scene.Add(std::move(HealthPooka));
		scene.Add(std::move(ScorePooka));
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