#pragma once
#include "Observer.h"
#include "Component.h"

namespace dae
{
	class TextComponent;
	class HealthComponent;
	class HealthDisplay final : public Component, public Observer
	{
	public:
		using Component::Component;

		void Init() override;
		void OnNotify(Event event) override;

		void SetHealthComponent(HealthComponent* healthComp) { m_Health = healthComp; }

	private:
		void UpdateDisplay();

		TextComponent* m_Display{ nullptr };
		HealthComponent* m_Health{ nullptr };
	};
}