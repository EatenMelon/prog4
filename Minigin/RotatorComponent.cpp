#include "RotatorComponent.h"
#include "GameObject.h"

void dae::RotatorComponent::Init()
{

}

void dae::RotatorComponent::Update(float deltaTime)
{
	constexpr float dubblePi{ 3.14159f * 2 };

	m_Angle += deltaTime * m_RotationVelocity;

	if (abs(m_Angle) >= dubblePi)
	{
		if (signbit(m_RotationVelocity))
		{
			m_Angle += dubblePi;
		}
		else
		{
			m_Angle -= dubblePi;
		}
	}

	float x{ cosf(m_Angle) * m_Radius };
	float y{ sinf(m_Angle) * m_Radius };

	GetOwner().SetLocalPosition(x, y);
}

void dae::RotatorComponent::SetRadius(float radius)
{
	m_Radius = radius;
}

void dae::RotatorComponent::SetSpeed(float speed)
{
	if (signbit(m_RotationVelocity))
	{
		m_RotationVelocity = -abs(speed);
	}
	else
	{
		m_RotationVelocity = abs(speed);
	}
}

void dae::RotatorComponent::SetDirection(RotationDirection direction)
{
	m_RotationVelocity = abs(m_RotationVelocity) * static_cast<float>(direction);
}
