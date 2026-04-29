#pragma once
#include "Component.h"
#include "Subject.h"

namespace dae
{
	class HealthComponent final : public Component, public Subject
	{
	public:
		using Component::Component;

		void Damage(int damage)
		{
			m_Health -= damage;
			Notify(Event::ReceivedDamage);
		}

		int GetHealth() const { return m_Health; }

	private:
		int m_Health{ 3 };

	};
}