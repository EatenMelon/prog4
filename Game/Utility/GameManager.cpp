#include "GameManager.h"
#include <algorithm>
#include <limits>

#include <SceneManager.h>
#include <InputManager.h>
#include <GameObject.h>
#include <TextComponent.h>
#include <RenderComponent.h>
#include <ResourceManager.h>
#include <SDL3/SDL.h>

#include <EnemyBehavior.h>
#include <Enemy.h>
#include <Harpoon.h>
#include <ScoreComponent.h>

#include <JoinGameCmd.h>
#include <GridMoveCmd.h>
#include <HarpoonCmd.h>
#include <SelectorCmds.h>

#include <ObjectSelector.h>
#include <Button.h>

void digdug::GameManager::Init()
{
	WipeGameData();

	m_SceneIds.clear();

	minigin::SceneManager::GetInstance().CreateScene([this](minigin::Scene& scene) { this->LoadStartMenu(scene); });
	m_MainMenuScene = minigin::SceneManager::GetInstance().GetSceneCount() - 1;

	for (int level{ 1 }; level <= m_LastLevel; ++level)
	{
		minigin::SceneManager::GetInstance().CreateScene
		(
			[level](minigin::Scene& scene)
			{
				const std::string file{ "Level" + std::to_string(level) + ".json" };
				const int requiredPlayers{ GetInstance().GetRequiredPlayersObjects() };
				digdug::LevelLoader::GetInstance().LoadLevel(scene, file, requiredPlayers);
			}
		);

		m_SceneIds.push_back(minigin::SceneManager::GetInstance().GetSceneCount() - 1);
	}

	LevelLoadedEvent loadedEvent{ nullptr, std::vector<minigin::GameObject*>(), std::vector<minigin::GameObject*>() };
	m_LevelLoadedHash = loadedEvent.GetEventHash();

	InflatablePoppedEvent poppedEvent{ nullptr };
	m_EnemyPoppedHash = poppedEvent.GetEventHash();

	ReceivedDamageEvent damagedEvent{ nullptr };
	m_PlayerTookDamageHash = damagedEvent.GetEventHash();

	LevelLoader::GetInstance().OnLevelLoadedEvent().Subscribe(this);

	m_JoinCommand = std::make_shared<JoinGameCmd>();

	ToMainMenu();
}

void digdug::GameManager::Quit()
{
	LevelLoader::GetInstance().OnLevelLoadedEvent().UnSubscribe(this);
}

void digdug::GameManager::ToMainMenu()
{
	m_CurrentLevel = -1;
	minigin::SceneManager::GetInstance().SetActiveScene(m_MainMenuScene);
}

void digdug::GameManager::StartGame(GameMode gameMode)
{
	WipeGameData();

	if (gameMode == GameMode::None)
	{
		std::cerr << "ERROR: GameManager::StartGame, cannot start game since a game is currently running!\n";
		return;
	}

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

int digdug::GameManager::GetRequiredPlayersObjects()
{
	int count = GetRequiredPlayerCount();

	if (m_CurrentMode == GameMode::Versus)
	{
		count /= 2;
	}

	return count;
}

void digdug::GameManager::OnNotify(const minigin::IEvent& event)
{
	if (event.GetEventHash() == m_LevelLoadedHash)
	{
		const auto& loadedEvent = static_cast<const digdug::LevelLoadedEvent&>(event);
		HandleLoadedEvent(loadedEvent);
		return;
	}

	if (event.GetEventHash() == m_EnemyPoppedHash)
	{
		const auto& loadedEvent = static_cast<const digdug::InflatablePoppedEvent&>(event);
		HandleEnemyPoppedEvent(loadedEvent);
		return;
	}

	if (event.GetEventHash() == m_PlayerTookDamageHash)
	{
		const auto& dmgEvent = static_cast<const digdug::ReceivedDamageEvent&>(event);
		HandleDamageEvent(dmgEvent);
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

	if (m_Players.size() >= m_PlayerObjects.size())
	{
		// add a enemy player
		if (m_CurrentMode != GameMode::Versus) return;

		constexpr size_t exceedingLimit{ (std::numeric_limits<size_t>::max)() };
		m_Players.emplace(playerId, exceedingLimit);
	}
	else
	{
		const auto slot = m_Players.size();
		m_Players.emplace(playerId, slot);
	}

	if (static_cast<int>(m_Players.size()) != playersRequired) return;

	m_JoinCommand->Enable(false);
	AssignCommandsToPlayers();
	StartWatchingPlayers();
	AddDisplays(minigin::SceneManager::GetInstance().GetActiveScene());
}

void digdug::GameManager::WipeGameData()
{
	m_Gird = nullptr;
	m_EnemyPlayer = nullptr;
	m_PlayersAlive = 0;
	m_CurrentMode = GameMode::None;
	m_CurrentLevel = -1;

	m_PlayerScores.clear();
	m_PlayerObjects.clear();
	m_Players.clear();
	m_Enemies.clear();
}

void digdug::GameManager::LoadStartMenu(minigin::Scene& scene)
{
	auto soloButton = AddButton(scene, "Play Singleplayer", []() { GetInstance().StartGame(GameMode::SinglePlayer); });
	auto coopButton = AddButton(scene, "Play Coop", []() { GetInstance().StartGame(GameMode::Coop); });
	auto versusButton = AddButton(scene, "Play Versus", []() { GetInstance().StartGame(GameMode::Versus); });

	if (soloButton == nullptr || coopButton == nullptr || versusButton == nullptr)
	{
		std::cout << "failed to add buttons to start menu!\n";
		return;
	}

	auto parent = std::make_unique<minigin::GameObject>();
	parent->SetLocalPosition(glm::vec3(350.f, 400.f, 0.f));

	soloButton->GetOwner().SetParent(parent.get());
	coopButton->GetOwner().SetParent(parent.get());
	versusButton->GetOwner().SetParent(parent.get());

	const glm::vec3 gaps{ 0.f, 100.f, 0.f };

	soloButton->GetOwner().SetLocalPosition(gaps * 0.f);
	coopButton->GetOwner().SetLocalPosition(gaps * 1.f);
	versusButton->GetOwner().SetLocalPosition(gaps * 2.f);

	scene.Add(std::move(parent));

	auto selector = std::make_unique<minigin::GameObject>();
	{
		auto renderComp = selector->AddComponent<minigin::RenderComponent>();
		if (renderComp == nullptr) return;

		renderComp->SetTexture("Sprites/Characters/TaizoHori.png");

		renderComp->MatchHeight(48.f);
		selector->SetLocalPosition(glm::vec3(-renderComp->GetSize().x, 0.f, 0.f));

		auto selectorComp = selector->AddComponent<digdug::ObjectSelector>();
		if (selectorComp == nullptr) return;

		selectorComp->AddSelectableObject(&soloButton->GetOwner());
		selectorComp->AddSelectableObject(&coopButton->GetOwner());
		selectorComp->AddSelectableObject(&versusButton->GetOwner());

		auto selectCmd = std::make_shared<digdug::SelectCmd>(selectorComp);
		auto selectCmdDelayed = std::make_shared<digdug::SelectCmd>(selectorComp, 0.25f);
		auto submitCmd= std::make_shared<digdug::SubmitCmd>(selectorComp);

		for (int id{ minigin::InputManager::GetKeyboardID() }; id < minigin::InputManager::GetGamepadLimit(); ++id)
		{
			if (id == minigin::InputManager::GetKeyboardID())
			{
				minigin::InputManager::GetInstance().BindInput("select", SDLK_W, minigin::KeyState::OnRelease, selectCmd, id, minigin::Direction::Up);
				minigin::InputManager::GetInstance().BindInput("select", SDLK_S, minigin::KeyState::OnRelease, selectCmd, id, minigin::Direction::Down);
				minigin::InputManager::GetInstance().BindInput("select", SDLK_SPACE, minigin::KeyState::OnDown, submitCmd, id);
				continue;
			}

			minigin::InputManager::GetInstance().BindInput("select", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, selectCmdDelayed, id);
			minigin::InputManager::GetInstance().BindInput("select", minigin::GamepadButton::SOUTH, minigin::KeyState::OnDown, submitCmd, id);
		}
	}

	scene.Add(std::move(selector));
}

digdug::Button* digdug::GameManager::AddButton(minigin::Scene& scene, const std::string& text, std::function<void()> onSubmit)
{
	auto fontButtons = minigin::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 48);
	SDL_Color textColor{ 255, 255, 255, 255 };

	auto obj = std::make_unique<minigin::GameObject>();

	auto buttonComp = obj->AddComponent<digdug::Button>();
	if (buttonComp == nullptr) return nullptr;
	
	buttonComp->SetSubmitFunction(onSubmit);
	
	auto renderComp = obj->AddComponent<minigin::RenderComponent>();
	if (renderComp == nullptr) return nullptr;

	auto textComp = obj->AddComponent<minigin::TextComponent>();
	if (textComp == nullptr) return nullptr;
	
	textComp->SetFont(fontButtons);
	textComp->SetColor(textColor);
	textComp->SetText(text);

	scene.Add(std::move(obj));

	return buttonComp;
}

void digdug::GameManager::NextLevel()
{
	++m_CurrentLevel;

	if (m_CurrentLevel >= m_LastLevel)
	{
		WipeGameData();
		minigin::SceneManager::GetInstance().SetActiveScene(m_MainMenuScene);
		return;
	}

	m_PlayersAlive = GetRequiredPlayerCount();
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

		for (size_t child{ 0 }; child < obj->GetChildCount(); ++obj)
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
	StartWatchingPlayers();
	AddDisplays(minigin::SceneManager::GetInstance().GetActiveScene());
}

void digdug::GameManager::HandleEnemyPoppedEvent(const InflatablePoppedEvent& event)
{
	if (m_CurrentMode == GameMode::Versus)
	{
		if(&m_EnemyPlayer->GetOwner() == event.GetPoppedObj())
		{
			auto healthComp = m_EnemyPlayer->GetOwner().GetComponent<digdug::HealthComponent>();

			if (healthComp->GetHealth() >= 0) return;
		}
	}

	auto itrEnemy = std::find_if
	(
		m_Enemies.begin(),
		m_Enemies.end(),
		[&](auto e)
		{
			return (&e->GetOwner() == event.GetPoppedObj());
		}
	);

	if (itrEnemy == m_Enemies.end()) return;
	m_Enemies.erase(itrEnemy);

	if (m_Enemies.empty())
	{
		NextLevel();
	}

	return;
}

void digdug::GameManager::HandleDamageEvent(const ReceivedDamageEvent& event)
{
	auto healthComp = event.GetVictim();

	if (healthComp == nullptr) return;
	if (healthComp->GetHealth() >= 0) return;

	for (auto [id, idx] : m_Players)
	{
		if (idx >= m_PlayerObjects.size()) continue;
		
		m_PlayerObjects[idx].first->Enable(false);
		m_PlayerObjects[idx].second->Enable(false);
		--m_PlayersAlive;
		EvaluateLivingPlayers();
		return;
	}
}

void digdug::GameManager::EvaluateLivingPlayers()
{
	switch (m_CurrentMode)
	{
	case digdug::GameManager::GameMode::Versus:
		if (m_PlayersAlive > 1) return;
		break;
	default:
		if (m_PlayersAlive > 0) return;
		break;
	}

	WipeGameData();
	minigin::SceneManager::GetInstance().SetActiveScene(m_MainMenuScene);
}

void digdug::GameManager::AssignCommandsToPlayers()
{
	const int playersRequired = GetRequiredPlayerCount();
	const int playerObjectsRequired = GetRequiredPlayersObjects();
	if (playersRequired <= 0) return;
	if (m_Players.size() > static_cast<size_t>(playersRequired))
	{
		std::cerr << "ERROR: GameManager::AssignCommandsToPlayers, player count exceeds limit!\n";
		return;
	}

	if (m_PlayerObjects.size() < static_cast<size_t>(playerObjectsRequired))
	{
		std::cerr << "ERROR: GameManager::AssignCommandsToPlayers, not enough player objects!\n";
		return;
	}

	size_t possessedPlayers{ 0 };
	for (const auto [id, objIndex] : m_Players)
	{
		if (possessedPlayers >= m_PlayerObjects.size())
		{
			PossessEnemy(id);
			continue;
		}

		PossessPlayer(id, objIndex);
		++possessedPlayers;
	}

	for (auto enemy : m_Enemies)
	{
		auto inflatable = enemy->GetOwner().GetComponent<Inflatable>();
		inflatable->PoppedEvent().Subscribe(this);

		if (enemy == m_EnemyPlayer) continue;
		enemy->Enable(true);
	}
}

void digdug::GameManager::PossessPlayer(int playerId, size_t objIdx)
{
	auto player = m_PlayerObjects[objIdx].first;
	auto harpoon = m_PlayerObjects[objIdx].second;

	auto renderComp = player->GetComponent<minigin::RenderComponent>();
	auto pos = player->GetLocalPosition() + glm::vec3(renderComp->GetSize() / 2.f, 0);

	auto moveCmd = std::make_shared<GridMoveCmd>(player, playerId, *m_Gird, 250.f, true);
	moveCmd->SetGridPosition(m_Gird->GetPosInGrid(pos));

	auto launchCmd = std::make_shared<digdug::HarpoonCmd>(playerId, harpoon, [](digdug::Harpoon* h) { h->Shoot(); });
	auto retractCmd = std::make_shared<digdug::HarpoonCmd>(playerId, harpoon, [](digdug::Harpoon* h) { h->Retract(); });

	harpoon->DisableDuringUse(moveCmd.get());

	if (playerId == minigin::InputManager::GetKeyboardID())
	{
		minigin::InputManager::GetInstance().BindInput("move", SDLK_W, minigin::KeyState::Pressed, moveCmd, playerId, minigin::Direction::Up);
		minigin::InputManager::GetInstance().BindInput("move", SDLK_A, minigin::KeyState::Pressed, moveCmd, playerId, minigin::Direction::Left);
		minigin::InputManager::GetInstance().BindInput("move", SDLK_S, minigin::KeyState::Pressed, moveCmd, playerId, minigin::Direction::Down);
		minigin::InputManager::GetInstance().BindInput("move", SDLK_D, minigin::KeyState::Pressed, moveCmd, playerId, minigin::Direction::Right);

		minigin::InputManager::GetInstance().BindInput("launch", SDLK_SPACE, minigin::KeyState::OnDown, launchCmd, playerId);
		minigin::InputManager::GetInstance().BindInput("retract", SDLK_SPACE, minigin::KeyState::OnRelease, retractCmd, playerId);
		return;
	}

	minigin::InputManager::GetInstance().BindInput("move", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, moveCmd, playerId);
	minigin::InputManager::GetInstance().BindInput("launch", minigin::GamepadButton::SOUTH, minigin::KeyState::OnDown, launchCmd, playerId);
	minigin::InputManager::GetInstance().BindInput("retract", minigin::GamepadButton::SOUTH, minigin::KeyState::OnRelease, retractCmd, playerId);
}

void digdug::GameManager::PossessEnemy(int playerId)
{
	if (m_Enemies.size() <= 0)
	{
		std::cerr << "ERROR: GameManager::JoinPlayer, enemy player cannot join since there are no enemies!\n";
		return;
	}

	minigin::GameObject* enemyPlayer{ nullptr };

	for (size_t idx{ 0 }; idx < m_Enemies.size(); ++idx)
	{
		auto& obj = m_Enemies[idx]->GetOwner();
		auto enemy = obj.GetComponent<Enemy>();

		if (enemy == nullptr) continue;
		if (!enemy->HasAttack()) continue;

		enemyPlayer = &enemy->GetOwner();

		m_EnemyPlayer = m_Enemies[idx];
		m_EnemyPlayer->Enable(false);

		m_Players.emplace(playerId, idx);
		break;
	}

	if (enemyPlayer == nullptr)
	{
		m_EnemyPlayer = m_Enemies[0];
		m_EnemyPlayer->Enable(false);

		enemyPlayer = &m_Enemies.front()->GetOwner();
	}

	auto renderComp = enemyPlayer->GetComponent<minigin::RenderComponent>();
	auto pos = enemyPlayer->GetLocalPosition() + glm::vec3(renderComp->GetSize() / 2.f, 0);

	auto moveCmd = std::make_shared<GridMoveCmd>(enemyPlayer, playerId, *m_Gird, 250.f, false);
	moveCmd->SetGridPosition(m_Gird->GetPosInGrid(pos));

	if (playerId == minigin::InputManager::GetKeyboardID())
	{
		minigin::InputManager::GetInstance().BindInput("move", SDLK_W, minigin::KeyState::Pressed, moveCmd, playerId, minigin::Direction::Up);
		minigin::InputManager::GetInstance().BindInput("move", SDLK_A, minigin::KeyState::Pressed, moveCmd, playerId, minigin::Direction::Left);
		minigin::InputManager::GetInstance().BindInput("move", SDLK_S, minigin::KeyState::Pressed, moveCmd, playerId, minigin::Direction::Down);
		minigin::InputManager::GetInstance().BindInput("move", SDLK_D, minigin::KeyState::Pressed, moveCmd, playerId, minigin::Direction::Right);
		return;
	}

	minigin::InputManager::GetInstance().BindInput("move", minigin::GamepadJoystick::LEFT_JOYSTICK, 0.5f, moveCmd, playerId);
}

void digdug::GameManager::StartWatchingPlayers()
{
	for (auto [player, harpoon] : m_PlayerObjects)
	{
		auto healthComp = player->GetComponent<digdug::HealthComponent>();

		if (healthComp == nullptr) continue;

		healthComp->TookDamageEvent().Subscribe(this);
	}

	if (m_EnemyPlayer == nullptr) return;

	auto healthComp = m_EnemyPlayer->GetOwner().AddComponent<digdug::HealthComponent>();
	if (healthComp == nullptr) return;

	healthComp->SetTargetType(KillingComponent::Target::Enemy);
	healthComp->TookDamageEvent().Subscribe(this);
}

void digdug::GameManager::AddDisplays(minigin::Scene& scene)
{
	std::vector<minigin::GameObject*> displays{};

	size_t spawned{ 0 };
	for (auto [id, objIdx] : m_Players)
	{
		if (spawned >= m_PlayerObjects.size())
		{
			auto health = AddHealthDisplay(scene, &m_EnemyPlayer->GetOwner(), spawned);

			if (health == nullptr) continue;
			displays.push_back(health);

			continue;
		}

		auto itr = m_PlayerScores.find(id);
		if (itr == m_PlayerScores.end())
		{
			m_PlayerScores.emplace(id, 0);
			itr = m_PlayerScores.find(id);
		}
		auto obj = m_PlayerObjects[objIdx].first;

		auto health = AddHealthDisplay(scene, obj, spawned);
		if (health == nullptr) continue;
		displays.push_back(health);

		auto score = AddScoreDisplay(scene, obj, spawned, itr->second);
		if (score == nullptr) continue;
		displays.push_back(score);

		++spawned;
	}

	auto board = std::make_unique<minigin::GameObject>();
	for (auto display : displays)
	{
		display->SetParent(board.get());
	}

	const float x{ m_Gird->GetSize().x + 50.f };

	board->SetLocalPosition(x, 100.f, 0.f);
	scene.Add(std::move(board));
}

minigin::GameObject* digdug::GameManager::AddScoreDisplay(minigin::Scene& scene, minigin::GameObject* obj, size_t index, int score)
{
	constexpr uint8_t textSize{ 24 };
	auto font = minigin::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", textSize);

	auto scoreComp = obj->GetComponent<digdug::ScoreComponent>();
	if (scoreComp == nullptr) return nullptr;

	scoreComp->SetScore(score);

	constexpr float gap{ 100.f };
	const float y{ index * gap + textSize };
	
	auto display = std::make_unique<minigin::GameObject>();
	display->AddComponent<minigin::RenderComponent>();
	display->SetLocalPosition(0.f, y, 0.f);

	auto textComp = display->AddComponent<minigin::TextComponent>();
	textComp->SetFont(font);
	textComp->SetText("...");

	if (textComp == nullptr) return nullptr;

	const std::string& message{ "Score P" + std::to_string(index) + ":   " };
	scoreComp->LinkTextComponent(textComp, message);

	auto ref = display.get();
	scene.Add(std::move(display));
	return ref;
}

minigin::GameObject* digdug::GameManager::AddHealthDisplay(minigin::Scene& scene, minigin::GameObject* obj, size_t index)
{
	constexpr uint8_t textSize{ 24 };
	auto font = minigin::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", textSize);

	auto healthComp = obj->GetComponent<digdug::HealthComponent>();
	if (healthComp == nullptr) return nullptr;

	constexpr float gap{ 100.f };
	const float y{ index * gap };

	auto display = std::make_unique<minigin::GameObject>();
	display->AddComponent<minigin::RenderComponent>();
	display->SetLocalPosition(0.f, y, 0.f);

	auto textComp = display->AddComponent<minigin::TextComponent>();
	textComp->SetFont(font);
	textComp->SetText("...");

	if (textComp == nullptr) return nullptr;

	const std::string& message{ "Lives P" + std::to_string(index) + ":   "};
	healthComp->LinkTextComponent(textComp, message);

	auto ref = display.get();
	scene.Add(std::move(display));
	return ref;
}