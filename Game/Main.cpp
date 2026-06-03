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
#include <ScoreComponent.h>
#include <Hitbox.h>

// input
#include "InputManager.h"

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
				SDL_Color color{};
				color.r = static_cast<unsigned char>(std::rand() % 255);
				color.g = static_cast<unsigned char>(std::rand() % 255);
				color.b = static_cast<unsigned char>(std::rand() % 255);

				text->SetFont(font);
				text->SetColor(color);
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
		TaizoHori->SetLocalPosition(300.f, 200.f);

		auto renderComp = TaizoHori->AddComponent<minigin::RenderComponent>();
		if (renderComp != nullptr)
		{
			renderComp->SetTexture("Sprites/TaizoHori.png");
			renderComp->SetUniformScale(3);
			renderComp->SetRotation(0);
			renderComp->SetHorizontalFlip(true);
		}

		auto HealthTaizoHori = std::make_unique<minigin::GameObject>();
		HealthTaizoHori->SetLocalPosition(15.f, 150.f);

		auto health = TaizoHori->AddComponent<digdug::HealthComponent>();
		if (health != nullptr)
		{
			auto renderCompDisplay = HealthTaizoHori->AddComponent<minigin::RenderComponent>();
			if (renderCompDisplay != nullptr)
			{
				auto text = HealthTaizoHori->AddComponent<minigin::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");

					health->LinkTextComponent(text);
				}
			}
		}

		auto ScoreTaizoHori = std::make_unique<minigin::GameObject>();
		ScoreTaizoHori->SetLocalPosition(15.f, 170.f);

		auto score = TaizoHori->AddComponent<digdug::ScoreComponent>();
		if (score != nullptr)
		{
			auto renderCompDisplay = ScoreTaizoHori->AddComponent<minigin::RenderComponent>();
			if (renderCompDisplay != nullptr)
			{
				auto text = ScoreTaizoHori->AddComponent<minigin::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");

					score->LinkTextComponent(text);
				}
			}
		}

		auto hitbox = TaizoHori->AddComponent<minigin::Hitbox>();
		if (hitbox != nullptr)
		{
			if (renderComp != nullptr)
			{
				auto size = renderComp->GetSize();
				hitbox->SetSize(size.x, size.y);
			}
			else
			{
				hitbox->SetSize(16, 16);
			}

			hitbox->HitEnterEvent().Subscrube(health);
			hitbox->HitExitEvent().Subscrube(score);
		}

		int playerID{ 0 };
		int keyboard{ -1 };
		auto moveCommand = std::make_shared<minigin::MoveCommand>(TaizoHori.get(), playerID, 250.f);
		auto moveCommandk = std::make_shared<minigin::MoveCommand>(TaizoHori.get(), keyboard, 250.f);

		minigin::InputManager::GetInstance().BindInput("Move", SDLK_W, minigin::KeyState::Pressed, moveCommandk, keyboard, minigin::Direction::Up);
		minigin::InputManager::GetInstance().BindInput("Move", SDLK_A, minigin::KeyState::Pressed, moveCommandk, keyboard, minigin::Direction::Left);
		minigin::InputManager::GetInstance().BindInput("Move", SDLK_S, minigin::KeyState::Pressed, moveCommandk, keyboard, minigin::Direction::Down);
		minigin::InputManager::GetInstance().BindInput("Move", SDLK_D, minigin::KeyState::Pressed, moveCommandk, keyboard, minigin::Direction::Right);
		
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, moveCommand, playerID);

		scene.Add(std::move(TaizoHori));
		scene.Add(std::move(HealthTaizoHori));
		scene.Add(std::move(ScoreTaizoHori));
	}

	auto Pooka = std::make_unique<minigin::GameObject>();
	{
		Pooka->SetLocalPosition(200.f, 200.f);

		auto renderComp = Pooka->AddComponent<minigin::RenderComponent>();
		if (renderComp != nullptr)
		{
			renderComp->SetTexture("Sprites/Pooka.png");
			renderComp->SetUniformScale(3);
		}

		auto HealthPooka = std::make_unique<minigin::GameObject>();
		HealthPooka->SetLocalPosition(15.f, 250.f);

		auto health = Pooka->AddComponent<digdug::HealthComponent>();
		if (health != nullptr)
		{
			auto renderCompDisplay = HealthPooka->AddComponent<minigin::RenderComponent>();
			if (renderCompDisplay != nullptr)
			{
				auto text = HealthPooka->AddComponent<minigin::TextComponent>();
				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");

					health->LinkTextComponent(text);
				}
			}
		}

		auto ScorePooka = std::make_unique<minigin::GameObject>();
		ScorePooka->SetLocalPosition(15.f, 270.f);

		auto score = Pooka->AddComponent<digdug::ScoreComponent>();
		if (score != nullptr)
		{
			auto renderCompDisplay = ScorePooka->AddComponent<minigin::RenderComponent>();
			if (renderCompDisplay != nullptr)
			{
				auto text = ScorePooka->AddComponent<minigin::TextComponent>();

				if (text != nullptr)
				{
					text->SetFont(smallFont);
					text->SetColor({ 255, 255, 0, 255 });
					text->SetText("...");

					score->LinkTextComponent(text);
				}
			}
		}

		auto hitbox = Pooka->AddComponent<minigin::Hitbox>();
		if (hitbox != nullptr)
		{
			if (renderComp != nullptr)
			{
				auto size = renderComp->GetSize();
				hitbox->SetSize(size.x, size.y);
			}
			else
			{
				hitbox->SetSize(16, 16);
			}

			hitbox->HitExitEvent().Subscrube(health);
			hitbox->HitEnterEvent().Subscrube(score);
		}

		int playerID{ 1 };
		auto moveCommand = std::make_shared<minigin::MoveCommand>(Pooka.get(), playerID, 250.f);

		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_UP, minigin::KeyState::Pressed, moveCommand, playerID, minigin::Direction::Up);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_LEFT, minigin::KeyState::Pressed, moveCommand, playerID, minigin::Direction::Left);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_DOWN, minigin::KeyState::Pressed, moveCommand, playerID, minigin::Direction::Down);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_RIGHT, minigin::KeyState::Pressed, moveCommand, playerID, minigin::Direction::Right);

		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, moveCommand, playerID);

		scene.Add(std::move(Pooka));
		scene.Add(std::move(HealthPooka));
		scene.Add(std::move(ScorePooka));
	}
}

static void DoubleLoad()
{
	load();
	load();
	load();
	load();
	load();
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
	engine.Run(DoubleLoad);

    return 0;
}