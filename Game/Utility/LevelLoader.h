#pragma once
#include <string>
#include <filesystem>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include <Scene.h>
#include <Singleton.h>
#include <Subject.h>
#include <Events.h>

namespace digdug::JsonLevelKeys
{
	constexpr std::string_view Tunnels{ "tunnels" };
	constexpr std::string_view Start{ "start" };
	constexpr std::string_view End{ "end" };
	constexpr std::string_view Axis{ "axis" };
	constexpr std::string_view AxisX{ "x" };
	constexpr std::string_view AxisY{ "y" };

	constexpr std::string_view Enemies{ "enemies" };
	constexpr std::string_view Position{ "pos" };
	constexpr std::string_view EnemyType{ "type" };
	constexpr std::string_view Fygar{ "Fygar" };
	constexpr std::string_view Pooka{ "Pooka" };

	constexpr std::string_view PlayerSpawnPoints{ "spawn_points" };
}

namespace minigin
{
	class GameObject;
}

namespace digdug
{
	class DirtGrid;
	class LevelLoadedEvent final : public minigin::PlainEvent
	{
	public:
		LevelLoadedEvent
		(
			DirtGrid* grid,
			const std::vector<minigin::GameObject*>& players,
			const std::vector<minigin::GameObject*>& enemies
		);

		DirtGrid* GetDirtGrid() const { return m_Grid; }

	private:
		DirtGrid* m_Grid{ nullptr };
		std::vector<minigin::GameObject*> m_Players{};
		std::vector<minigin::GameObject*> m_Enemies{};
	};

	class LevelLoader final : public minigin::Singleton<LevelLoader>
	{
	public:
		void Init(const std::filesystem::path& root);
		bool AddLevel(const std::string& file);
		void LoadLevel(minigin::Scene& scene, const std::string& file, int requiredPlayers);
		minigin::Subject& OnLevelLoadedEvent() { return m_OnLevelLoaded; }

	private:
		minigin::Subject m_OnLevelLoaded{};

		struct TunnelData
		{
			glm::ivec2 start{};
			glm::ivec2 end{};
			char axis{ 'x' };
		};

		struct EnemyData
		{
			glm::ivec2 position{};

			enum class Type { Pooka, Fygar };
			Type type{ Type::Pooka };
		};

		struct LevelData
		{
			std::vector<TunnelData> tunnels{};
			std::vector<EnemyData> enemies{};
			std::vector<glm::ivec2> playerSpawnPositions{};
		};

		struct SpritePack
		{
			SpritePack(const std::string& solid, const std::string& ghost, const std::string& inflating)
				: solidSprite{ solid }, ghostSprite{ ghost }, inflatingSpite{ inflating } { }

			std::string solidSprite{};
			std::string ghostSprite{};
			std::string inflatingSpite{};
		};

		const std::unordered_map<EnemyData::Type, SpritePack> m_SpritesMap
		{
			{
				EnemyData::Type::Pooka, 
				SpritePack
				(
					"Sprites/Characters/Pooka.png",
					"Sprites/Characters/SpookyPooka.png",
					"Sprites/Characters/InflatingPooka.png"
				)
			},
			{
				EnemyData::Type::Fygar,
				SpritePack
				(
					"Sprites/Characters/Fygar.png",
					"Sprites/Characters/SpookyFygar.png", 
					"Sprites/Characters/InflatingFygar.png"
				)
			}
		};

		DirtGrid* AddDirtGrid(minigin::Scene& scene, float cellSize);
		minigin::GameObject* AddEnemy(minigin::Scene& scene, DirtGrid* grid, const EnemyData& enemyData);
		minigin::GameObject* AddPlayer(minigin::Scene& scene, DirtGrid* grid, const glm::ivec2 position);

		std::filesystem::path m_Root{};
		std::unordered_map<std::string, LevelData> m_Levels{};
	};
}