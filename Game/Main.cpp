#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

// engine core
#include <Minigin.h>
#include <Scene.h>
#include <SceneManager.h>
#include <ResourceManager.h>

// engine components
#include <RenderComponent.h>
#include <TextComponent.h>

// my components
#include <FpsCounterComponent.h>
#include <HealthComponent.h>
#include <HealthDisplay.h>
#include <ScoreComponent.h>
#include <ScoreDisplay.h>

// input
#include "InputManager.h"
#include <DamageCommand.h>
#include <ScoreCommand.h>

#include <filesystem>

namespace fs = std::filesystem;

static void load()
{
	auto& scene = minigin::SceneManager::GetInstance().CreateScene();
	auto font = minigin::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 36);
	auto smallFont = minigin::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 18);
	
	auto background = std::make_unique<minigin::GameObject>();
	{
		auto renderComponent = background->AddComponent<minigin::RenderComponent>();
		if (renderComponent != nullptr) renderComponent->SetTexture("Sprites/background.png");

		scene.Add(std::move(background));
	}

	auto logo = std::make_unique<minigin::GameObject>();
	{
		logo->SetLocalPosition(358.f, 180.f);

		auto renderComp = logo->AddComponent<minigin::RenderComponent>();
		if (renderComp != nullptr) renderComp->SetTexture("Sprites/logo.png");

		scene.Add(std::move(logo));
	}

	auto title = std::make_unique<minigin::GameObject>();
	{
		title->SetLocalPosition(292.f, 20.f);

		auto renderComp = title->AddComponent<minigin::RenderComponent>();

		if (renderComp != nullptr)
		{
			auto text = title->AddComponent<minigin::TextComponent>();

			if (text != nullptr)
			{
				text->SetFont(font);
				text->SetColor({ 255, 255, 0, 255 });
				text->SetText("Programming 4 Assignment");
			}
		}
		scene.Add(std::move(title));
	}
	
	auto fpsCounter = std::make_unique<minigin::GameObject>();
	{
		fpsCounter->SetLocalPosition(20.f, 20.f);

		auto renderComp = fpsCounter->AddComponent<minigin::RenderComponent>();

		if (renderComp != nullptr)
		{
			auto text = fpsCounter->AddComponent<minigin::TextComponent>();

			if (text != nullptr)
			{
				text->SetText("...");
				text->SetFont(font);
				text->SetColor({ 255, 255, 0, 255 });

				fpsCounter->AddComponent<digdug::FpsCounterComponent>();
			}
		}

		scene.Add(std::move(fpsCounter));
	}

	auto controlsTaizoHori = std::make_unique<minigin::GameObject>();
	{
		controlsTaizoHori->SetLocalPosition(15.f, 80.f);

		auto renderComp = controlsTaizoHori->AddComponent<minigin::RenderComponent>();

		if (renderComp != nullptr)
		{
			auto text = controlsTaizoHori->AddComponent<minigin::TextComponent>();

			if (text != nullptr)
			{
				text->SetFont(smallFont);
				text->SetColor({ 255, 255, 0, 255 });
				text->SetText("Use WASD to move Taizo Hori, C to inflict damage, and X to Pick up carrots.");
			}
		}
		scene.Add(std::move(controlsTaizoHori));
	}

	auto controlsPooka = std::make_unique<minigin::GameObject>();
	{
		controlsPooka->SetLocalPosition(15.f, 110.f);

		auto renderComp = controlsPooka->AddComponent<minigin::RenderComponent>();

		if (renderComp != nullptr)
		{
			auto text = controlsPooka->AddComponent<minigin::TextComponent>();

			if (text != nullptr)
			{
				text->SetFont(smallFont);
				text->SetColor({ 255, 255, 0, 255 });
				text->SetText("Use the D-Pad to move Pooka, X to inflict damage, and A to Pick up carrots.");
			}
		}
		scene.Add(std::move(controlsPooka));
	}

	auto TaizoHori = std::make_unique<minigin::GameObject>();
	{
		auto HealthTaizoHori = std::make_unique<minigin::GameObject>();
		auto healthDisplayTaizoHori = HealthTaizoHori->AddComponent<digdug::HealthDisplay>();
		{
			HealthTaizoHori->SetLocalPosition(15.f, 150.f);

			auto renderComp = HealthTaizoHori->AddComponent<minigin::RenderComponent>();

			if (renderComp != nullptr)
			{
				auto text = HealthTaizoHori->AddComponent<minigin::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");
				}
			}
		}

		auto ScoreTaizoHori = std::make_unique<minigin::GameObject>();
		auto scoreDisplayTaizoHori = ScoreTaizoHori->AddComponent<digdug::ScoreDisplay>();
		{
			ScoreTaizoHori->SetLocalPosition(15.f, 170.f);

			auto renderComp = ScoreTaizoHori->AddComponent<minigin::RenderComponent>();

			if (renderComp != nullptr)
			{
				auto text = ScoreTaizoHori->AddComponent<minigin::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");
				}
			}
		}

		TaizoHori->SetLocalPosition(300.f, 200.f);

		int playerID{ 0 };
		int keyboard{ -1 };
		auto moveCommand = std::make_shared<minigin::MoveCommand>(TaizoHori.get(), playerID, 250.f);
		auto moveCommandk = std::make_shared<minigin::MoveCommand>(TaizoHori.get(), keyboard, 250.f);

		minigin::InputManager::GetInstance().BindInput("Move", SDLK_W, minigin::KeyState::Pressed, moveCommandk, keyboard, minigin::Direction::Up);
		minigin::InputManager::GetInstance().BindInput("Move", SDLK_A, minigin::KeyState::Pressed, moveCommandk, keyboard, minigin::Direction::Left);
		minigin::InputManager::GetInstance().BindInput("Move", SDLK_S, minigin::KeyState::Pressed, moveCommandk, keyboard, minigin::Direction::Down);
		minigin::InputManager::GetInstance().BindInput("Move", SDLK_D, minigin::KeyState::Pressed, moveCommandk, keyboard, minigin::Direction::Right);
		
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, moveCommand, playerID);

		auto renderComp = TaizoHori->AddComponent<minigin::RenderComponent>();
		if (renderComp != nullptr) renderComp->SetTexture("Sprites/TaizoHori.png");

		auto health = TaizoHori->AddComponent<digdug::HealthComponent>();
		if (health != nullptr && healthDisplayTaizoHori != nullptr)
		{
			health->Subscrube(healthDisplayTaizoHori);
			healthDisplayTaizoHori->SetHealthComponent(health);
		}

		auto damageCommand = std::make_shared<digdug::DamageCommand>(TaizoHori.get(), keyboard, health);
		minigin::InputManager::GetInstance().BindInput("Damage", SDLK_C, minigin::KeyState::OnDown, damageCommand, keyboard);

		auto score = TaizoHori->AddComponent<digdug::ScoreComponent>();
		if (score != nullptr && scoreDisplayTaizoHori != nullptr)
		{
			score->Subscrube(scoreDisplayTaizoHori);
			scoreDisplayTaizoHori->SetScoreComponent(score);

		}

		auto scoreCommand = std::make_shared<digdug::ScoreCommand>(TaizoHori.get(), keyboard, score);
		minigin::InputManager::GetInstance().BindInput("Score", SDLK_X, minigin::KeyState::OnDown, scoreCommand, keyboard);

		scene.Add(std::move(TaizoHori));
		scene.Add(std::move(HealthTaizoHori));
		scene.Add(std::move(ScoreTaizoHori));
	}

	auto Pooka = std::make_unique<minigin::GameObject>();
	{
		auto HealthPooka = std::make_unique<minigin::GameObject>();
		auto healthDisplayPooka = HealthPooka->AddComponent<digdug::HealthDisplay>();
		{
			HealthPooka->SetLocalPosition(15.f, 210.f);

			auto renderComp = HealthPooka->AddComponent<minigin::RenderComponent>();

			if (renderComp != nullptr)
			{
				auto text = HealthPooka->AddComponent<minigin::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");
				}
			}
		}

		auto ScorePooka = std::make_unique<minigin::GameObject>();
		auto scoreDisplayPooka = ScorePooka->AddComponent<digdug::ScoreDisplay>();
		{
			ScorePooka->SetLocalPosition(15.f, 230.f);

			auto renderComp = ScorePooka->AddComponent<minigin::RenderComponent>();

			if (renderComp != nullptr)
			{
				auto text = ScorePooka->AddComponent<minigin::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");
				}
			}
		}

		Pooka->SetLocalPosition(200.f, 200.f);

		int playerID{ 1 };
		auto moveCommand = std::make_shared<minigin::MoveCommand>(Pooka.get(), playerID, 250.f);

		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_UP, minigin::KeyState::Pressed, moveCommand, playerID, minigin::Direction::Up);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_LEFT, minigin::KeyState::Pressed, moveCommand, playerID, minigin::Direction::Left);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_DOWN, minigin::KeyState::Pressed, moveCommand, playerID, minigin::Direction::Down);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_RIGHT, minigin::KeyState::Pressed, moveCommand, playerID, minigin::Direction::Right);

		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, moveCommand, playerID);

		auto renderComp = Pooka->AddComponent<minigin::RenderComponent>();
		if (renderComp != nullptr) renderComp->SetTexture("Sprites/Pooka.png");

		auto health = Pooka->AddComponent<digdug::HealthComponent>();
		if (health != nullptr)
		{
			health->Subscrube(healthDisplayPooka);
			healthDisplayPooka->SetHealthComponent(health);
		}

		auto damageCommand = std::make_shared<digdug::DamageCommand>(Pooka.get(), playerID, health);
		minigin::InputManager::GetInstance().BindInput("Damage", minigin::GamepadButton::WEST, minigin::KeyState::OnDown, damageCommand, playerID);

		auto score = Pooka->AddComponent<digdug::ScoreComponent>();
		if (score != nullptr)
		{
			score->Subscrube(scoreDisplayPooka);
			scoreDisplayPooka->SetScoreComponent(score);
		}

		auto scoreCommand = std::make_shared<digdug::ScoreCommand>(Pooka.get(), playerID, score);
		minigin::InputManager::GetInstance().BindInput("Score", minigin::GamepadButton::SOUTH, minigin::KeyState::OnDown, scoreCommand, playerID);

		scene.Add(std::move(Pooka));
		scene.Add(std::move(HealthPooka));
		scene.Add(std::move(ScorePooka));
	}
}

int main(int, char*[])
{
#if defined(WIN32) && defined(_DEBUG)
	// open console window in debug mode on windows
	AllocConsole();

	// assinging the correct steams to the console
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);
#endif

#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	minigin::Minigin engine(data_location);
	engine.Run(load);

    return 0;
}