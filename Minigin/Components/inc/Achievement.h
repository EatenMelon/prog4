#pragma once

#if USE_STEAMWORKS
#pragma warning (push)
#pragma warning (disable:4996)
#include <steam_api.h>
#pragma warning (pop)
#endif

#include "ScoreComponent.h"
#include "Observer.h"

namespace dae
{
	class Achievement final : public Component, public Observer
	{
	public:
		using Component::Component;
		

		void Init() override
		{
			m_pSteamUserStats = SteamUserStats();
		}

		void OnNotify(Event event) override
		{
			if (m_HasAquiredAchivement) return;
			if (event != Event::ReceivedPoints) return;

			constexpr int requiredPoints{ 500 };

			if (m_Score->GetScore() < requiredPoints) return;

			m_HasAquiredAchivement = true;

			// set the achievemnt
			m_pSteamUserStats->SetAchievement("ACH_WIN_ONE_GAME");

			// send achievemnt to steam (for the cool notification)
			m_pSteamUserStats->StoreStats();
		}

		void SetScoreComponent(ScoreComponent* pScoreComponent)
		{
			m_Score = pScoreComponent;
		}

	private:
		ISteamUserStats* m_pSteamUserStats{ nullptr };
		ScoreComponent* m_Score{ nullptr };
		bool m_HasAquiredAchivement{ false };
	};
}