#include "LevelLoader.h"

#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include <GameObject.h>
#include <ResourceManager.h>
#include <RenderComponent.h>
#include <Hitbox.h>

#include <DirtGrid.h>
#include <AimComponent.h>

#include <Enemy.h>
#include <Attack.h>
#include <Inflatable.h>
#include <EnemyBehavior.h>
#include <KillingComponent.h>

#include <HealthComponent.h>
#include <ScoreComponent.h>
#include <Harpoon.h>

using Json = nlohmann::json;

digdug::LevelLoadedEvent::LevelLoadedEvent
(
	DirtGrid* grid,
	const std::vector<minigin::GameObject*>& players,
	const std::vector<minigin::GameObject*>& enemies
)
	: PlainEvent("LevelLoaded")
	, m_Grid{ grid }
	, m_Players{ players }
	, m_Enemies{ enemies }
{
}

void digdug::LevelLoader::Init(const std::filesystem::path& root)
{
	m_Root = root;
}

bool digdug::LevelLoader::AddLevel(const std::string& file)
{
	const auto path{ m_Root / file };

	std::ifstream fs{ path.string() };

	if (fs.fail()) return false;

	Json jsonFile = Json::parse(fs);
	if (m_Levels.contains(path.string()))
	{
		std::cout << "WARNING: failed to add level: " << path << " => has already been loaded!\n";
		return false;
	}

	LevelData newLevel{};

	for (const auto& obj : jsonFile[JsonLevelKeys::Tunnels])
	{
		const auto& start = obj[JsonLevelKeys::Start];
		const auto& end = obj[JsonLevelKeys::End];

		TunnelData newTunnel{};
		newTunnel.start.x = start[JsonLevelKeys::AxisX];
		newTunnel.start.y = start[JsonLevelKeys::AxisY];
		newTunnel.end.x = end[JsonLevelKeys::AxisX];
		newTunnel.end.y = end[JsonLevelKeys::AxisY];
		newTunnel.axis = obj[JsonLevelKeys::Axis].get<std::string>().front();

		newLevel.tunnels.push_back(newTunnel);
	}

	for (const auto& obj : jsonFile[JsonLevelKeys::Enemies])
	{
		EnemyData newEnemy{};

		std::string type = obj[JsonLevelKeys::EnemyType];
		
		if (type == JsonLevelKeys::Fygar)
		{
			newEnemy.type = EnemyData::Type::Fygar;
		}
		else if (type == JsonLevelKeys::Pooka)
		{
			newEnemy.type = EnemyData::Type::Pooka;
		}
		else
		{
			continue;
		}

		const auto& pos = obj[JsonLevelKeys::Position];
		newEnemy.position.x = pos[JsonLevelKeys::AxisX];
		newEnemy.position.y = pos[JsonLevelKeys::AxisY];

		newLevel.enemies.push_back(newEnemy);
	}

	for (const auto& obj : jsonFile[JsonLevelKeys::PlayerSpawnPoints])
	{
		glm::ivec2 pos{};
		pos.x = obj[JsonLevelKeys::AxisX];
		pos.y = obj[JsonLevelKeys::AxisY];

		newLevel.playerSpawnPositions.push_back(pos);
	}

	if (newLevel.tunnels.empty() && newLevel.enemies.empty())
	{
		std::cout << "ERROR: failed to add level: " << path << " => level was empty!\n";
		return false;
	}

	m_Levels.emplace(file, newLevel);
	return true;
}

void digdug::LevelLoader::LoadLevel(minigin::Scene& scene, const std::string& file, int requiredPlayers)
{
	if (requiredPlayers <= 0)
	{
		std::cerr << "ERROR: LevelLoader::LoadLevel, failed to load level for " << requiredPlayers << " players!\n";
		return;
	}

	auto it = m_Levels.find(file);

	if (it == m_Levels.end())
	{
		if (!AddLevel(file))
		{
			std::cerr << "ERROR: LevelLoader::LoadLevel, failed to load level since level could not be added!\n";
			return;
		}

		it = m_Levels.find(file);
	}

	const auto& level = it->second;
	auto dirtGrid = AddDirtGrid(scene, 48.f);

	if (dirtGrid == nullptr)
	{
		std::cerr << "ERROR: LevelLoader::LoadLevel, failed to add dirt grid to scene, level can't be loaded!\n";
		return;
	}

	for (const auto& tunnel : level.tunnels)
	{
		dirtGrid->Dig(tunnel.start, tunnel.end, tunnel.axis);
	}

	std::vector<minigin::GameObject*> enemies{};
	for (const auto& enemy : level.enemies)
	{
		auto newObj = AddEnemy(scene, dirtGrid, enemy);

		if (newObj == nullptr) continue;
		enemies.push_back(newObj);
	}

	if (level.playerSpawnPositions.empty())
	{
		std::cerr << "ERROR: LevelLoader::LoadLevel, no player can be spawned in this level since there are no swpawn positions!\n";
		return;
	}

	if (static_cast<int>(level.playerSpawnPositions.size()) < requiredPlayers)
	{
		std::cerr << "ERROR: LevelLoader::LoadLevel, not enough spawn positions for all players!\n";
		return;
	}

	std::vector<minigin::GameObject*> players{};

	for (int sp{ 0 }; sp < requiredPlayers; ++sp)
	{
		const auto pos = level.playerSpawnPositions[sp];
		auto newObj = AddPlayer(scene, dirtGrid, pos);

		if (newObj == nullptr) continue;
		players.push_back(newObj);
	}

	if (static_cast<int>(players.size()) != requiredPlayers)
	{
		std::cerr << "ERROR: LevelLoader::LoadLevel, not enough players where spawned!\n";
		return;
	}

	// Now i can be sure that all components are on the enemy gameobjects, so less checks won't hurt
	for (const auto& enemy : enemies)
	{
		auto enemyComp = enemy->GetComponent<Enemy>();
		
		for (const auto& player : players)
		{
			enemyComp->AddPossibleTarget(player);
		}
	}

	LevelLoadedEvent event{ dirtGrid, players, enemies };
	m_OnLevelLoaded.Notify(event);
}

digdug::DirtGrid* digdug::LevelLoader::AddDirtGrid(minigin::Scene& scene, float cellSize)
{
	auto obj = std::make_unique<minigin::GameObject>();

	auto grid = obj->AddComponent<digdug::DirtGrid>();

	if (grid == nullptr)
	{
		std::cerr << "ERROR: LevelLoader::AddDirtGrid, Failed to add dirt grid!";
		return nullptr;
	}

	grid->SetCellSize(cellSize);
	
	auto sandTile = minigin::ResourceManager::GetInstance().LoadTexture("Sprites/Tiles/SandTile.png");
	grid->SetTileTexture(digdug::DirtGrid::Depth::Surface, sandTile);
	grid->SetTileTexture(digdug::DirtGrid::Depth::TopSoil, sandTile);

	auto dirtTile = minigin::ResourceManager::GetInstance().LoadTexture("Sprites/Tiles/DirtTile.png");
	grid->SetTileTexture(digdug::DirtGrid::Depth::SubSoil, dirtTile);
	
	auto stoneTile = minigin::ResourceManager::GetInstance().LoadTexture("Sprites/Tiles/StoneTile.png");
	grid->SetTileTexture(digdug::DirtGrid::Depth::Stone, stoneTile);
	
	auto bedrockTile = minigin::ResourceManager::GetInstance().LoadTexture("Sprites/Tiles/BedrockTile.png");
	grid->SetTileTexture(digdug::DirtGrid::Depth::Bedrock, bedrockTile);

	scene.Add(std::move(obj));

	return grid;
}

minigin::GameObject* digdug::LevelLoader::AddEnemy(minigin::Scene& scene, DirtGrid* grid, const EnemyData& enemyData)
{
	auto obj = std::make_unique<minigin::GameObject>();

	auto renderComp = obj->AddComponent<minigin::RenderComponent>();
	if (renderComp == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add render component!\n";
		return nullptr;
	}

	const auto& itrSprites = m_SpritesMap.find(enemyData.type);
	if (itrSprites == m_SpritesMap.end())
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't find matching sprites for enemy type!\n";
		return nullptr;
	}

	const auto& sprites = itrSprites->second;
	renderComp->SetTexture(sprites.solidSprite);
	renderComp->MatchWidth(grid->GetCellSize());

	auto aimComp = obj->AddComponent<digdug::AimComponent>();
	if (aimComp == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add aim component!\n";
		return nullptr;
	}

	aimComp->LockAxis(glm::bvec2{ false, true });

	auto hitbox = obj->AddComponent<minigin::Hitbox>();
	if (hitbox == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add hitbox component!\n";
		return nullptr;
	}

	auto killComp = obj->AddComponent<digdug::KillingComponent>();
	if (killComp == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add killing component!\n";
		return nullptr;
	}

	killComp->AddTarget(KillingComponent::Target::Player);

	hitbox->SetBounds(renderComp->GetSize());
	hitbox->SetShrink(renderComp->GetSize().x / 10.f);

	auto inflatable = obj->AddComponent<digdug::Inflatable>();
	if (inflatable == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add render component!\n";
		return nullptr;
	}

	const auto& itrScores = m_ScoreValues.find(enemyData.type);
	if (itrScores == m_ScoreValues.end())
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't matching score value for enemy type!\n";
		return nullptr;
	}

	inflatable->SetScoreValue(itrScores->second);

	inflatable->SetSpriteSheet(sprites.inflatingSpite);

	auto enemy = obj->AddComponent<digdug::Enemy>();
	if (enemy == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add enemy component!\n";
		return nullptr;
	}

	enemy->SetDefaultSprite(sprites.solidSprite);
	enemy->SetGhostSprite(sprites.ghostSprite);

	switch (enemyData.type)
	{
	case digdug::LevelLoader::EnemyData::Type::Fygar:
	{
		enemy->SetAttack(std::make_shared<digdug::FireAttack>());
	}
		break;

	default:
		break;
	}

	auto enemyBehavior = obj->AddComponent<digdug::EnemyBehavior>();
	if (enemyBehavior == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add enemy behavior component!\n";
		return nullptr;
	}

	enemyBehavior->SetGrid(grid);
	enemyBehavior->SetPositionInGrid(enemyData.position);

	auto ref = obj.get();
	scene.Add(std::move(obj));

	return ref;
}

minigin::GameObject* digdug::LevelLoader::AddPlayer(minigin::Scene& scene, DirtGrid* grid, const glm::ivec2 position)
{
	auto playerObj = std::make_unique<minigin::GameObject>();
	{
		auto renderComp = playerObj->AddComponent<minigin::RenderComponent>();
		if (renderComp == nullptr)
		{
			std::cout << "WARNING: LevelLoader::AddPlayer, failed to add player to scene, couldn't add render component!\n";
			return nullptr;
		}

		renderComp->SetTexture("Sprites/Characters/TaizoHori.png");
		renderComp->MatchWidth(grid->GetCellSize());

		auto aimComp = playerObj->AddComponent<digdug::AimComponent>();
		if (aimComp == nullptr)
		{
			std::cout << "WARNING: LevelLoader::AddPlayer, failed to add player to scene, couldn't add aim component to player!\n";
			return nullptr;
		}

		auto hitbox = playerObj->AddComponent<minigin::Hitbox>();
		if (hitbox == nullptr)
		{
			std::cout << "WARNING: LevelLoader::AddPlayer, failed to add player to scene, couldn't add hitbox component to player!\n";
			return nullptr;
		}

		hitbox->SetBounds(renderComp->GetSize());
		hitbox->SetShrink(renderComp->GetSize().x / 10.f);

		auto healthComp = playerObj->AddComponent<digdug::HealthComponent>();
		if (healthComp == nullptr)
		{
			std::cout << "WARNING: LevelLoader::AddPlayer, failed to add player to scene, couldn't add hitbox component to player!\n";
			return nullptr;
		}

		auto scoreComp = playerObj->AddComponent<digdug::ScoreComponent>();
		if (scoreComp == nullptr)
		{
			std::cout << "WARNING: LevelLoader::AddPlayer, failed to add player to scene, couldn't add hitbox component to player!\n";
			return nullptr;
		}
	}

	const auto localPos = grid->GetCellLocalPos(position);
	playerObj->SetLocalPosition(localPos);

	auto harpoonObj = std::make_unique<minigin::GameObject>();
	{
		auto harpoonComp = harpoonObj->AddComponent<digdug::Harpoon>();
		if (harpoonComp == nullptr)
		{
			std::cout << "WARNING: LevelLoader::AddPlayer, failed to add player to scene, couldn't add harpoon component to harpoon!\n";
			return nullptr;
		}

		auto user = playerObj.get();
		harpoonComp->EquipOnUser(*user);
		harpoonComp->AddDirtdGrid(grid);
		harpoonComp->SetHarpoonSprite("Sprites/Attacks/Harpoon.png");

		auto hitbox = harpoonObj->AddComponent<minigin::Hitbox>();
		if (hitbox == nullptr)
		{
			std::cout << "WARNING: LevelLoader::AddPlayer, failed to add player to scene, couldn't add hitbox component to harpoon!\n";
			return nullptr;
		}
	}

	auto ref = playerObj.get();
	scene.Add(std::move(harpoonObj));
	scene.Add(std::move(playerObj));

	return ref;
}