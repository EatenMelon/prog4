#pragma once
#include <glm/glm.hpp>

namespace dae
{
	class Transform final
	{
	public:
		Transform(float x = 0.f, float y = 0.f, float z = 0.f);

		const glm::vec3& GetPosition() const { return m_position; }
		void SetPosition(float x, float y, float z = 0.f);
		void SetPosition(const glm::vec3& position);
	private:
		glm::vec3 m_position;
	};
}
