#pragma once
#include <unordered_map>
#include <memory>

#include <Scene.h>
#include <Observer.h>
#include <Singleton.h>

#include <LevelLoader.h>
#include <DirtGrid.h>

namespace minigin
{
	class GameObject;
}

namespace digdug
{
	class EnemyBehavior;
	class Harpoon;
	class JoinGameCmd;
	class GameManager final : public minigin::Singleton<GameManager>, public minigin::IObserver
	{
	public:
		enum class GameMode { None, SinglePlayer, Coop, Versus };

		void Init();
		void Quit();
		void StartGame(GameMode gameMode);
		int GetRequiredPlayerCount();

		void OnNotify(const minigin::IEvent& event) override;
		void JoinPlayer(int playerId);

	private:
		void NextLevel();
		void HandleLoadedEvent(const LevelLoadedEvent& event);
		void AssignCommandsToPlayers();

		GameMode m_CurrentMode{ GameMode::None };

		int m_CurrentLevel{ -1 };
		const int m_LastLevel{ 3 };
		std::vector<size_t> m_SceneIds{};

		unsigned int m_LevelLoadedHash{ 0 };

		DirtGrid* m_Gird{ nullptr };
		std::vector<EnemyBehavior*> m_Enemies{};
		std::unordered_map<int, size_t> m_Players{};
		std::vector<std::pair<minigin::GameObject*, Harpoon*>> m_PlayerObjects{};

		std::shared_ptr<JoinGameCmd> m_JoinCommand{};
	};
}