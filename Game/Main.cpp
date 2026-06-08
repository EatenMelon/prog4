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
#include <DirtGrid.h>
#include <AimComponent.h>
#include <Inflatable.h>

#include <GridMoveCmd.h>
#include <HarpoonCmd.h>

// input
#include "InputManager.h"

#include <filesystem>
#include <Renderer.h>
#include <Harpoon.h>

namespace fs = std::filesystem;

static void LoadTestScene(minigin::Scene& scene)
{
	std::cout << "Loading scene: " << scene.GetSceneId() << "\n";

	auto font = minigin::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 36);
	auto smallFont = minigin::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 18);

	float width{ 0 };
	digdug::DirtGrid* dirtGrid{ nullptr };

	auto sandbox = std::make_unique<minigin::GameObject>();
	{
		auto gridComp = sandbox->AddComponent<digdug::DirtGrid>();

		if (gridComp != nullptr)
		{
			auto sandTile = minigin::ResourceManager::GetInstance().LoadTexture("Sprites/Tiles/SandTile.png");
			auto dirtTile = minigin::ResourceManager::GetInstance().LoadTexture("Sprites/Tiles/DirtTile.png");
			auto stoneTile = minigin::ResourceManager::GetInstance().LoadTexture("Sprites/Tiles/StoneTile.png");
			auto bedrockTile = minigin::ResourceManager::GetInstance().LoadTexture("Sprites/Tiles/BedrockTile.png");

			gridComp->SetCellSize(16.f * 3.f);
			gridComp->SetTileTexture(digdug::DirtGrid::Depth::Surface, *sandTile.get());
			gridComp->SetTileTexture(digdug::DirtGrid::Depth::TopSoil, *sandTile.get());
			gridComp->SetTileTexture(digdug::DirtGrid::Depth::SubSoil, *dirtTile.get());
			gridComp->SetTileTexture(digdug::DirtGrid::Depth::Stone, *stoneTile.get());
			gridComp->SetTileTexture(digdug::DirtGrid::Depth::Bedrock, *bedrockTile.get());

			width = gridComp->GetSize().x;

			dirtGrid = gridComp;

			gridComp->Dig(glm::ivec2{ 1, 8 }, glm::ivec2{ 12, 8 }, 'x');
			gridComp->Dig(glm::ivec2{ 12, 2 }, glm::ivec2{ 12, 12 }, 'x');
		}

		scene.Add(std::move(sandbox));
	}

	auto fpsCounter = std::make_unique<minigin::GameObject>();
	{
		fpsCounter->SetLocalPosition(width + 20.f, 20.f);

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

	auto TaizoHori = std::make_unique<minigin::GameObject>();
	{
		TaizoHori->SetLocalPosition(300.f, 200.f);

		auto renderComp = TaizoHori->AddComponent<minigin::RenderComponent>();
		if (renderComp != nullptr)
		{
			renderComp->SetTexture("Sprites/Characters/TaizoHori.png");
			renderComp->SetUniformScale(3);
		}

		auto HealthTaizoHori = std::make_unique<minigin::GameObject>();
		HealthTaizoHori->SetLocalPosition(width + 15.f, 150.f);

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
		ScoreTaizoHori->SetLocalPosition(width + 15.f, 170.f);

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
				hitbox->SetBounds(size.x, size.y);
				hitbox->SetShrink(size.x / 6);
			}
			else
			{
				hitbox->SetBounds(16, 16);
			}

			hitbox->HitEnterEvent().Subscribe(health);
			hitbox->HitExitEvent().Subscribe(score);
		}

		auto aimComp = TaizoHori->AddComponent<digdug::AimComponent>();
		if (aimComp != nullptr)
		{
			aimComp->SetDirection(minigin::Direction::Right);
		}

		scene.Add(std::move(HealthTaizoHori));
		scene.Add(std::move(ScoreTaizoHori));
	}

	digdug::Harpoon* myHarpoon{ nullptr };

	auto Harpoon = std::make_unique<minigin::GameObject>();
	{
		auto harpoonComp = Harpoon->AddComponent<digdug::Harpoon>();
		
		if (harpoonComp != nullptr)
		{
			harpoonComp->EquipOnUser(*TaizoHori.get());
			harpoonComp->SetHarpoonSprite("Sprites/Attacks/Harpoon.png");

			int taizoID{ 0 };
			auto launchCmd = std::make_shared<digdug::HarpoonCmd>(taizoID, harpoonComp, [](digdug::Harpoon* h) { h->Shoot(); });
			auto retractCmd = std::make_shared<digdug::HarpoonCmd>(taizoID, harpoonComp, [](digdug::Harpoon* h) { h->Retract(); });

			minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::SOUTH, minigin::KeyState::OnDown, launchCmd, taizoID);
			minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::SOUTH, minigin::KeyState::OnRelease, retractCmd, taizoID);

			harpoonComp->AddDirtdGrid(dirtGrid);
			myHarpoon = harpoonComp;
		}

		Harpoon->AddComponent<minigin::Hitbox>();

		scene.Add(std::move(Harpoon));
	}

	auto Pooka = std::make_unique<minigin::GameObject>();
	{
		Pooka->SetLocalPosition(200.f, 200.f);

		auto renderComp = Pooka->AddComponent<minigin::RenderComponent>();
		if (renderComp != nullptr)
		{
			renderComp->SetTexture("Sprites/Characters/Pooka.png");
			renderComp->SetUniformScale(3);
		}

		auto HealthPooka = std::make_unique<minigin::GameObject>();
		HealthPooka->SetLocalPosition(width + 15.f, 250.f);

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
		ScorePooka->SetLocalPosition(width + 15.f, 270.f);

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
				hitbox->SetBounds(size.x, size.y);
				hitbox->SetShrink(size.x / 6);
			}

			hitbox->HitExitEvent().Subscribe(health);
			hitbox->HitEnterEvent().Subscribe(score);
		}

		auto aimComp = Pooka->AddComponent<digdug::AimComponent>();
		if (aimComp != nullptr)
		{
			aimComp->SetDirection(minigin::Direction::Up);
			aimComp->LockAxis(glm::bvec2{ false, true });
		}

		auto inflate = Pooka->AddComponent<digdug::Inflatable>();
		if (inflate != nullptr)
		{
			inflate->SetSpriteSheet("Sprites/Characters/InflatingPooka.png");
		}

		scene.Add(std::move(HealthPooka));
		scene.Add(std::move(ScorePooka));
	}

	if (dirtGrid != nullptr)
	{
		int taizoID{ 0 };
		auto taizoMove = std::make_shared<digdug::GridMoveCmd>(TaizoHori.get(), taizoID, *dirtGrid, 250.f);

		if (myHarpoon != nullptr)
		{
			myHarpoon->DisableDuringUse(taizoMove.get());
		}

		taizoMove->SetGridPosition(glm::ivec2{ 8, 8 });

		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_UP, minigin::KeyState::Pressed, taizoMove, taizoID, minigin::Direction::Up);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_LEFT, minigin::KeyState::Pressed, taizoMove, taizoID, minigin::Direction::Left);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_DOWN, minigin::KeyState::Pressed, taizoMove, taizoID, minigin::Direction::Down);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_RIGHT, minigin::KeyState::Pressed, taizoMove, taizoID, minigin::Direction::Right);

		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, taizoMove, taizoID);

		int pookaID{ 1 };
		auto pookaMove = std::make_shared<digdug::GridMoveCmd>(Pooka.get(), pookaID, *dirtGrid, 250.f, false);

		pookaMove->SetGridPosition(glm::ivec2{ 12, 3 });

		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_UP, minigin::KeyState::Pressed, pookaMove, pookaID, minigin::Direction::Up);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_LEFT, minigin::KeyState::Pressed, pookaMove, pookaID, minigin::Direction::Left);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_DOWN, minigin::KeyState::Pressed, pookaMove, pookaID, minigin::Direction::Down);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::DPAD_RIGHT, minigin::KeyState::Pressed, pookaMove, pookaID, minigin::Direction::Right);

		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, pookaMove, pookaID);
		
		scene.Add(std::move(TaizoHori));
		scene.Add(std::move(Pooka));
	}

	minigin::Renderer::GetInstance().SetBackgroundColor(SDL_Color{ 0, 0, 50, 255 });
}

static void LoadAllScenes()
{
	minigin::SceneManager::GetInstance().CreateScene(LoadTestScene);
	minigin::SceneManager::GetInstance().CreateScene(LoadTestScene);
	minigin::SceneManager::GetInstance().CreateScene(LoadTestScene);
	minigin::SceneManager::GetInstance().CreateScene(LoadTestScene);
	minigin::SceneManager::GetInstance().CreateScene(LoadTestScene);
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
	minigin::Minigin engine(data_location, 1080, 720);
	engine.Run(LoadAllScenes);

    return 0;
}