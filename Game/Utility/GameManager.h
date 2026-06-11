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
		void LoadStartMenu(minigin::Scene& scene);
		Button* AddButton(minigin::Scene& scene, const std::string& text, std::function<void()> onSubmit);
		
		void NextLevel();
		void HandleLoadedEvent(const LevelLoadedEvent& event);

		void AssignCommandsToPlayers();

		void PossessPlayer(int playerId, size_t objIdx);
		void PossessEnemy(int playerId);

		//void AddDisplays(minigin::Scene& scene);
		//void AddScoreDisplay(minigin::Scene& scene, minigin::GameObject* obj);
		//void AddHealthDisplay(minigin::Scene& scene, minigin::GameObject* obj);

		GameMode m_CurrentMode{ GameMode::None };

		size_t m_MainMenuScene{ 0 };
		//size_t m_ScoreboardScene{ 0 };

		int m_CurrentLevel{ -1 };
		const int m_LastLevel{ 3 };
		std::vector<size_t> m_SceneIds{};

		unsigned int m_LevelLoadedHash{ 0 };
		unsigned int m_EnemyPoppedHash{ 0 };

		DirtGrid* m_Gird{ nullptr };
		std::vector<EnemyBehavior*> m_Enemies{};
		EnemyBehavior* m_EnemyPlayer{ nullptr };

		std::unordered_map<int, size_t> m_Players{};
		std::vector<std::pair<minigin::GameObject*, Harpoon*>> m_PlayerObjects{};

		std::shared_ptr<JoinGameCmd> m_JoinCommand{};
	};
}