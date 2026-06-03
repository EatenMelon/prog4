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
	class ScoreComponent final : public minigin::Component, public minigin::IObserver
	{
	public:
		using Component::Component;

		void Start() override;
		void OnNotify(const minigin::IEvent& event) override;

		void LinkTextComponent(minigin::TextComponent* comp);
		int GetScore() const { return m_Score; }

	private:
		void UpdateDisplay();

		minigin::TextComponent* m_Display{ nullptr };

		int m_Score{ 0 };
		unsigned int m_HitEventHash{ 0 };
	};
}