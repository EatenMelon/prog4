#pragma once
#include <glm/glm.hpp>
#include "Component.h"

namespace dae
{
	class CharacterController final : public Component
	{
	public:
		using Component::Component;

		void Update(float deltaTime) override;
		void GuiRender() const override;

		void SetMovementSpeed(float speed);
		void OnMove(const glm::vec2& direction);

	private:
		glm::vec2 m_MoveDirection{};
		float m_MovementSpeed{ 10.f };
	};
}