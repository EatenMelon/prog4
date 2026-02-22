#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class RotatorComponent final : public Component
	{
	public:
		using Component::Component;

		void Init() override;
		void Update(float deltaTime) override;

		void SetRadius(float radius);
		void SetSpeed(float speed);

	private:
		float m_TimePassed{ 0.f };

		float m_Radius{ 10.f };
		float m_RotationSpeed{ 1.f };
		glm::vec3 m_CenterPos{ };
	};
}