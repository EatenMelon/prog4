#pragma once
#include "Component.h"
#include "Subject.h"

namespace minigin
{
	class Component;
}

namespace digdug
{
	class ScoreComponent final : public minigin::Component, public minigin::Subject
	{
	public:
		using Component::Component;

		void AddPoints(int points)
		{
			m_Score += points;
			auto event = minigin::PlainEvent("points");

			Notify(&event);
		}

		int GetScore() const { return m_Score; }

	private:
		int m_Score{ 0 };

	};
}