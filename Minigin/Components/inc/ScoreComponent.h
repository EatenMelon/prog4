#pragma once
#include "Component.h"
#include "Subject.h"

namespace dae
{
	class ScoreComponent final : public Component, public Subject
	{
	public:
		using Component::Component;

		void AddPoints(int points)
		{
			m_Score += points;
			Notify(Event::ReceivedPoints);
		}

		int GetScore() const { return m_Score; }

	private:
		int m_Score{ 0 };

	};
}