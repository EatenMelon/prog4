#pragma once
#include "Observer.h"
#include "Component.h"

namespace minigin
{
	class TextComponent;
}

namespace digdug
{
	class HealthComponent;
	class HealthDisplay final : public minigin::Component, public minigin::IObserver
	{
	public:
		using Component::Component;

		void Init() override;
		void OnNotify(minigin::IEvent event) override;

		void SetHealthComponent(HealthComponent* healthComp) { m_Health = healthComp; }

	private:
		void UpdateDisplay();

		minigin::TextComponent* m_Display{ nullptr };
		HealthComponent* m_Health{ nullptr };
	};
}