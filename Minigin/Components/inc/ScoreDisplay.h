#pragma once
#include "Observer.h"
#include "Component.h"

namespace dae
{
	class TextComponent;
	class ScoreComponent;
	class ScoreDisplay final : public Component, public Observer
	{
	public:
		using Component::Component;

		void Init() override;
		void OnNotify(Event event) override;

		void SetScoreComponent(ScoreComponent* scoreComp) { m_Score = scoreComp; }

	private:
		void UpdateDisplay();

		TextComponent* m_Display{ nullptr };
		ScoreComponent* m_Score{ nullptr };
	};
}