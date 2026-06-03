#pragma once
#include "Observer.h"
#include "Component.h"

namespace minigin
{
	class TextComponent;
}

namespace digdug
{
	
	class ScoreComponent;
	class ScoreDisplay final : public minigin::Component, public minigin::IObserver
	{
	public:
		using Component::Component;

		void Start() override;
		void OnNotify(const minigin::IEvent& event) override;

		void SetScoreComponent(ScoreComponent* scoreComp) { m_Score = scoreComp; }

	private:
		void UpdateDisplay();

		minigin::TextComponent* m_Display{ nullptr };
		ScoreComponent* m_Score{ nullptr };
	};
}