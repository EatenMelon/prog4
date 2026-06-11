#pragma once
#include <unordered_map>
#include <memory>

#include <Scene.h>
#include <Observer.h>
#include <Singleton.h>

#include <LevelLoader.h>
#include <DirtGrid.h>
#include <HealthComponent.h>
#include <Inflatable.h>

namespace minigin
{
	class GameObject;
}

namespace digdug
{
	class Harpoon;
	class EnemyBehavior;
	class JoinGameCmd;
	class Button;
	class GameManager final : public minigin::Singleton<GameManager>, public minigin::IObserver
	{
	public:
		enum class GameMode { None, SinglePlayer, Coop, Versus };

		void Init();
		void Quit();

		void ToMainMenu();
		void StartGame(GameMode gameMode);

		int GetRequiredPlayerCount();
		int GetRequiredPlayersObjects();

		void OnNotify(const minigin::IEvent& event) override;
		void JoinPlayer(int playerId);

	private:
		void WipeGameData();

		void LoadStartMenu(minigin::Scene& scene);
		Button* AddButton(minigin::Scene& scene, const std::string& text, std::function<void()> onSubmit);
		
		void NextLevel();
		void HandleLoadedEvent(const LevelLoadedEvent& event);
		void HandleEnemyPoppedEvent(const InflatablePoppedEvent& event);
		void HandleDamageEvent(const ReceivedDamageEvent& event);
		void EvaluateLivingPlayers();

		void AssignCommandsToPlayers();

		void PossessPlayer(int playerId, size_t objIdx);
		void PossessEnemy(int playerId);

		void StartWatchingPlayers();

		void AddDisplays(minigin::Scene& scene);
		minigin::GameObject* AddScoreDisplay(minigin::Scene& scene, minigin::GameObject* obj, size_t index, int score);
		minigin::GameObject* AddHealthDisplay(minigin::Scene& scene, minigin::GameObject* obj, size_t index);

		GameMode m_CurrentMode{ GameMode::None };

		size_t m_MainMenuScene{ 0 };

		int m_CurrentLevel{ -1 };
		const int m_LastLevel{ 3 };
		std::vector<size_t> m_SceneIds{};

		unsigned int m_LevelLoadedHash{ 0 };
		unsigned int m_EnemyPoppedHash{ 0 };
		unsigned int m_PlayerTookDamageHash{ 0 };

		DirtGrid* m_Gird{ nullptr };
		EnemyBehavior* m_EnemyPlayer{ nullptr };
		std::vector<EnemyBehavior*> m_Enemies{};
		int m_PlayersAlive{ 0 };

		std::unordered_map<int, size_t> m_Players{};
		std::unordered_map<int, int> m_PlayerScores{};
		std::vector<std::pair<minigin::GameObject*, Harpoon*>> m_PlayerObjects{};

		std::shared_ptr<JoinGameCmd> m_JoinCommand{};
	};
}