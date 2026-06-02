#pragma once
#include "Component.h"
#include "Subject.h"

namespace digdug
{
	class HealthComponent final : public minigin::Component, public minigin::Subject
	{
	public:
		using Component::Component;

		void Damage(int damage)
		{
			m_Health -= damage;

			auto event = minigin::PlainEvent("damage");

			Notify(&event);
		}

		int GetHealth() const { return m_Health; }

	private:
		int m_Health{ 3 };

	};
}