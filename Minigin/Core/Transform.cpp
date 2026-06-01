#include "Transform.h"

minigin::Transform::Transform(float x, float y, float z)
	: m_position{ x, y, z }
{
}

void minigin::Transform::SetPosition(const float x, const float y, const float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void minigin::Transform::SetPosition(const glm::vec3& position) 
{ 
	m_position = position; 
}