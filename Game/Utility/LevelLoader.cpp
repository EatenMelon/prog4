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

using Json = nlohmann::json;

void digdug::LevelLoader::Init(const std::filesystem::path& root)
{
	m_Root = root;
}

bool digdug::LevelLoader::AddLevel(const std::string& file)
{
	const auto path{ m_Root / file };

	std::ifstream fs{ path.string() };
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

	if (newLevel.tunnels.empty() && newLevel.enemies.empty())
	{
		std::cout << "ERROR: failed to add level: " << path << " => level was empty!\n";
		return false;
	}

	m_Levels.emplace(file, newLevel);
	return true;
}

void digdug::LevelLoader::LoadLevel(minigin::Scene& scene, const std::string& file)
{
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

	for (const auto& enemy : level.enemies)
	{
		AddEnemy(scene, dirtGrid, enemy);
	}
	
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

void digdug::LevelLoader::AddEnemy(minigin::Scene& scene, DirtGrid* grid, const EnemyData& enemyData)
{
	auto obj = std::make_unique<minigin::GameObject>();

	auto renderComp = obj->AddComponent<minigin::RenderComponent>();
	if (renderComp == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add render component!\n";
		return;
	}

	const auto& itr = m_SpritesMap.find(enemyData.type);
	if (itr == m_SpritesMap.end())
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't matching sprites for enemy type!\n";
		return;
	}

	const auto& sprites = itr->second;
	renderComp->SetTexture(sprites.solidSprite);
	renderComp->MatchWidth(grid->GetCellSize());

	auto aimComp = obj->AddComponent<digdug::AimComponent>();
	if (aimComp == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add aim component!\n";
		return;
	}

	aimComp->LockAxis(glm::bvec2{ false, true });

	auto hitbox = obj->AddComponent<minigin::Hitbox>();
	if (hitbox == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add hitbox component!\n";
		return;
	}

	hitbox->SetBounds(renderComp->GetSize());

	auto inflatable = obj->AddComponent<digdug::Inflatable>();
	if (inflatable == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add render component!\n";
		return;
	}

	inflatable->SetSpriteSheet(sprites.inflatingSpite);

	auto enemy = obj->AddComponent<digdug::Enemy>();
	if (enemy == nullptr)
	{
		std::cout << "WARNING: LevelLoader::AddEnemy, failed to add enemy to scene, couldn't add enemy component!\n";
		return;
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
		return;
	}

	enemyBehavior->SetGrid(grid);
	enemyBehavior->SetPositionInGrid(enemyData.position);

	scene.Add(std::move(obj));
}