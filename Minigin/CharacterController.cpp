#include "CharacterController.h"
#include "GameObject.h"
#include "imgui.h"

void dae::CharacterController::Update(float deltaTime)
{
	glm::vec3 pos = GetOwner().GetLocalPosition();

	pos.x += m_MoveDirection.x * m_MovementSpeed * deltaTime;
	pos.y += m_MoveDirection.y * m_MovementSpeed * deltaTime;

	GetOwner().SetLocalPosition(pos);

	m_MoveDirection = { 0, 0 };
}

void dae::CharacterController::GuiRender() const
{
#ifdef _DEBUG
	ImGui::Begin("character controller");

	glm::vec3 pos = GetOwner().GetLocalPosition();

	ImGui::Text("(%p) pos: %f, %f", this, pos.x, pos.y);

	ImGui::End();
#endif
}

void dae::CharacterController::SetMovementSpeed(float speed)
{
	m_MovementSpeed = speed;
}

void dae::CharacterController::OnMove(const glm::vec2& direction)
{
	m_MoveDirection = direction;
}
