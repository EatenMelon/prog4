#include "GameManager.h"

#include <SceneManager.h>
#include <InputManager.h>
#include <GameObject.h>
#include <SDL3/SDL.h>

#include <EnemyBehavior.h>
#include <Harpoon.h>

#include <JoinGameCmd.h>
#include <GridMoveCmd.h>
#include <HarpoonCmd.h>

void digdug::GameManager::Init()
{
	m_Gird = nullptr;
	m_PlayerObjects.clear();
	m_Players.clear();
	m_Enemies.clear();

	m_SceneIds.clear();
	m_CurrentLevel = 0;

	for (int level{ 1 }; level <= m_LastLevel; ++level)
	{
		minigin::SceneManager::GetInstance().CreateScene
		(
			[level](minigin::Scene& scene)
			{
				const std::string file{ "Level" + std::to_string(level) + ".json" };
				const int requiredPlayers{ GetInstance().GetRequiredPlayerCount() };
				digdug::LevelLoader::GetInstance().LoadLevel(scene, file, requiredPlayers);
			}
		);

		m_SceneIds.push_back(minigin::SceneManager::GetInstance().GetSceneCount() - 1);
	}

	LevelLoadedEvent event{ nullptr, std::vector<minigin::GameObject*>(), std::vector<minigin::GameObject*>() };
	m_LevelLoadedHash = event.GetEventHash();

	LevelLoader::GetInstance().OnLevelLoadedEvent().Subscribe(this);

	m_JoinCommand = std::make_shared<JoinGameCmd>();
}

void digdug::GameManager::Quit()
{
	LevelLoader::GetInstance().OnLevelLoadedEvent().UnSubscribe(this);
}

void digdug::GameManager::StartGame(GameMode gameMode)
{
	m_Gird = nullptr;
	m_PlayerObjects.clear();
	m_Players.clear();
	m_Enemies.clear();

	if (gameMode == GameMode::None)
	{
		std::cerr << "ERROR: GameManager::StartGame, cannot start game since a game is currently running!\n";
		return;
	}

	m_CurrentLevel = -1;
	m_CurrentMode = gameMode;
	NextLevel();
}

int digdug::GameManager::GetRequiredPlayerCount()
{
	constexpr int maxPlayers{ 2 };
	constexpr int minPlayers{ 1 };
	constexpr int error{ -1 };

	switch (m_CurrentMode)
	{
	case digdug::GameManager::GameMode::SinglePlayer:
		return minPlayers;
		break;

	case digdug::GameManager::GameMode::Coop:
	case digdug::GameManager::GameMode::Versus:
		return maxPlayers;
		break;

	default:
		return error;
		break;
	}
}

void digdug::GameManager::OnNotify(const minigin::IEvent& event)
{
	if (event.GetEventHash() == m_LevelLoadedHash)
	{
		const auto& loadedEvent = static_cast<const digdug::LevelLoadedEvent&>(event);
		HandleLoadedEvent(loadedEvent);
		return;
	}
}

void digdug::GameManager::JoinPlayer(int playerId)
{
	const int playersRequired = GetRequiredPlayerCount();
	if (playersRequired <= 0) return;
	if (m_Players.size() >= static_cast<size_t>(playersRequired)) return;

	auto itr = m_Players.find(playerId);
	if (itr != m_Players.end()) return;

	if (m_Players.size() >= m_PlayerObjects.size()) return;

	const auto slot = m_Players.size();
	m_Players.emplace(playerId, slot);

	if (static_cast<int>(m_Players.size()) != playersRequired) return;

	m_JoinCommand->Enable(false);
	AssignCommandsToPlayers();
}

void digdug::GameManager::NextLevel()
{
	++m_CurrentLevel;
	minigin::SceneManager::GetInstance().SetActiveScene(m_SceneIds[m_CurrentLevel]);
}

void digdug::GameManager::HandleLoadedEvent(const LevelLoadedEvent& event)
{
	// get all enemy behaviors
	m_Enemies.clear();
	for (auto obj : event.GetEnemies())
	{
		auto enemy = obj->GetComponent<digdug::EnemyBehavior>();
		if (enemy == nullptr) continue;
		m_Enemies.push_back(enemy);
	}

	// get all players and their harpoons
	m_PlayerObjects.clear();
	for (auto obj : event.GetPlayers())
	{
		Harpoon* harpoon{ nullptr };

		for (int child{ 0 }; child < obj->GetChildCount(); ++obj)
		{
			auto harpoonObj = obj->GetChildAt(child);

			auto harpoonComp = harpoonObj->GetComponent<Harpoon>();

			if (harpoonComp == nullptr) continue;

			harpoon = harpoonComp;
			break;
		}

		if (harpoon == nullptr) continue;

		m_PlayerObjects.emplace_back(obj, harpoon);
	}

	m_Gird = event.GetDirtGrid();

	// allow players to join the game
	if (static_cast<int>(m_Players.size()) != GetRequiredPlayerCount())
	{
		m_JoinCommand->Enable(true);
		for (int id{ minigin::InputManager::GetKeyboardID() }; id < minigin::InputManager::GetGamepadLimit(); ++id)
		{
			if (id == minigin::InputManager::GetKeyboardID())
			{
				minigin::InputManager::GetInstance().BindInput("Join", SDLK_E, minigin::KeyState::OnRelease, m_JoinCommand, id);
				continue;
			}

			minigin::InputManager::GetInstance().BindInput("Join", minigin::GamepadButton::SOUTH, minigin::KeyState::OnRelease, m_JoinCommand, id);
		}

		for (auto enemy : m_Enemies)
		{
			enemy->Enable(false);
		}

		return;
	}

	AssignCommandsToPlayers();
}

void digdug::GameManager::AssignCommandsToPlayers()
{
	const int playersRequired = GetRequiredPlayerCount();
	if (playersRequired <= 0) return;
	if (m_Players.size() > static_cast<size_t>(playersRequired))
	{
		std::cerr << "ERROR: GameManager::AssignCommandsToPlayers, player count exceeds limit!\n";
		return;
	}

	if (m_PlayerObjects.size() < static_cast<size_t>(playersRequired))
	{
		std::cerr << "ERROR: GameManager::AssignCommandsToPlayers, not enough player objects!\n";
		return;
	}

	for (const auto [id, objIndex] : m_Players)
	{
		auto player = m_PlayerObjects[objIndex].first;
		auto harpoon = m_PlayerObjects[objIndex].second;

		auto renderComp = player->GetComponent<minigin::RenderComponent>();
		auto pos = player->GetLocalPosition() + glm::vec3(renderComp->GetSize() / 2.f, 0);

		auto moveCmd = std::make_shared<GridMoveCmd>(player, id, *m_Gird, 250.f, true);
		moveCmd->SetGridPosition(m_Gird->GetPosInGrid(pos));

		auto launchCmd = std::make_shared<digdug::HarpoonCmd>(id, harpoon, [](digdug::Harpoon* h) { h->Shoot(); });
		auto retractCmd = std::make_shared<digdug::HarpoonCmd>(id, harpoon, [](digdug::Harpoon* h) { h->Retract(); });

		harpoon->DisableDuringUse(moveCmd.get());

		if (id == minigin::InputManager::GetKeyboardID())
		{
			minigin::InputManager::GetInstance().BindInput("move", SDLK_W, minigin::KeyState::Pressed, moveCmd, id, minigin::Direction::Up);
			minigin::InputManager::GetInstance().BindInput("move", SDLK_A, minigin::KeyState::Pressed, moveCmd, id, minigin::Direction::Left);
			minigin::InputManager::GetInstance().BindInput("move", SDLK_S, minigin::KeyState::Pressed, moveCmd, id, minigin::Direction::Down);
			minigin::InputManager::GetInstance().BindInput("move", SDLK_D, minigin::KeyState::Pressed, moveCmd, id, minigin::Direction::Right);

			minigin::InputManager::GetInstance().BindInput("Move", SDLK_SPACE, minigin::KeyState::OnDown, launchCmd, id);
			minigin::InputManager::GetInstance().BindInput("Move", SDLK_SPACE, minigin::KeyState::OnRelease, retractCmd, id);
			continue;
		}

		minigin::InputManager::GetInstance().BindInput("move", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, moveCmd, id);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::SOUTH, minigin::KeyState::OnDown, launchCmd, id);
		minigin::InputManager::GetInstance().BindInput("Move", minigin::GamepadButton::SOUTH, minigin::KeyState::OnRelease, retractCmd, id);
	}

	for (auto enemy : m_Enemies)
	{
		enemy->Enable(true);
	}
}