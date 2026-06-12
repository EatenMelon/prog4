#pragma once
#include "Component.h"
#include <Observer.h>
#include "Events.h"
#include <string>
#include <Subject.h>
#include <KillingComponent.h>

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

		void LinkTextComponent(minigin::TextComponent* comp, const std::string& message);
		void TakeDamage();
		void SetTargetType(KillingComponent::Target target) { m_TargetType = target; }

		int GetHealth() const { return m_Health; }

		minigin::Subject& TookDamageEvent() { return m_TookDamageEvent; };

	private:
		void UpdateDisplay();

		minigin::Subject m_TookDamageEvent{};

		minigin::TextComponent* m_Display{ nullptr };
		std::string m_Message{};

		int m_Health{ 0 };
		const int m_MaxHealth{ 3 };
		unsigned int m_HitEventHash{ 0 };

		KillingComponent::Target m_TargetType{ KillingComponent::Target::Player };

		// TODO add grace periode, and reset harpoon on respawn
	};

	class ReceivedDamageEvent final : public minigin::PlainEvent
	{
	public:
		ReceivedDamageEvent(digdug::HealthComponent* victim)
			: minigin::PlainEvent("ReceivedDamage")
			, m_Victim{ victim }
		{  }

		digdug::HealthComponent* GetVictim() const { return m_Victim; }

	private:
		digdug::HealthComponent* m_Victim{};
	};
}