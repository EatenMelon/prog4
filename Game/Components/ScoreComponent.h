#pragma once
#include "Component.h"
#include <Observer.h>
#include "Events.h"

namespace minigin
{
	class TextComponent;
}

namespace digdug
{
	class ScoreComponent final : public minigin::Component
	{
	public:
		using Component::Component;

		void Start() override;

		void LinkTextComponent(minigin::TextComponent* comp, const std::string& message);

		int GetScore() const { return m_Score; }
		void SetScore(int score);
		void AddScore(int addedScore);

	private:
		void UpdateDisplay();

		minigin::TextComponent* m_Display{ nullptr };
		std::string m_Message{};

		int m_Score{ 0 };
	};
}