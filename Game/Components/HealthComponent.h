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
	class HealthComponent final : public minigin::Component, public minigin::IObserver
	{
	public:
		using Component::Component;

		void Start() override;
		void OnNotify(const minigin::IEvent& event) override;

		void LinkTextComponent(minigin::TextComponent* comp);
		int GetHealth() const { return m_Health; }

	private:
		void UpdateDisplay();

		minigin::TextComponent* m_Display{ nullptr };

		int m_Health{ 3 };
		unsigned int m_HitEventHash{ 0 };
	};
}