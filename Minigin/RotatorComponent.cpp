#include "RotatorComponent.h"
#include "GameObject.h"

void dae::RotatorComponent::Init()
{
	m_CenterPos = GetOwner().GetWorldPosition();
}

void dae::RotatorComponent::Update(float deltaTime)
{
	m_TimePassed += deltaTime;

	float x{ m_CenterPos.x + cosf(m_TimePassed * m_RotationSpeed) * m_Radius };
	float y{ m_CenterPos.y + sinf(m_TimePassed * m_RotationSpeed) * m_Radius };

	GetOwner().SetLocalPosition(x, y);
}

void dae::RotatorComponent::SetRadius(float radius)
{
	m_Radius = radius;
}

void dae::RotatorComponent::SetSpeed(float speed)
{
	m_RotationSpeed = speed;
}
