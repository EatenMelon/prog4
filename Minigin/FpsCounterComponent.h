#pragma once
#include "Component.h"

namespace dae
{
	class FpsCounterComponent final : public Component
	{
	public:
		// reuse the constructor of the base component class
		using Component::Component;

		void Update(float deltaTime) override;

	private:

		float m_UntilTextUpdate{ 0.f };

		int m_UpdateTimes{ 12 };
	};
}