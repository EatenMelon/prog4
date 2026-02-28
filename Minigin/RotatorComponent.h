#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	enum class RotationDirection { CW = 1, CCW = -1 };
	class RotatorComponent final : public Component
	{
	public:
		using Component::Component;

		void Init() override;
		void Update(float deltaTime) override;

		void SetRadius(float radius);
		void SetSpeed(float speed);
		void SetDirection(RotationDirection direction);

	private:
		float m_Angle{ 0.f };

		float m_Radius{ 10.f };
		float m_RotationVelocity{ 1.f };
	};
}