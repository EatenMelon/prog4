#pragma once
#include <Component.h>
#include <MiniginTypes.h>
#include <glm/glm.hpp>

namespace minigin
{
	class RenderComponent;
}

namespace digdug
{
	class AimComponent final : public minigin::Component
	{
	public:
		using minigin::Component::Component;

		void Start() override;
		void Update(float) override;

		void SetDirection(minigin::Direction direction);
		minigin::Direction GetDirection() const { return m_AimDirection; }

		glm::ivec2 GetDirectionAsVector() const;
		static glm::ivec2 GetDirectionAsVector(minigin::Direction dir);

		void LockAxis(const glm::bvec2& axisLocks);

	private:
		minigin::Direction m_AimDirection{ minigin::Direction::Right };
		minigin::Direction m_LastDirection{ minigin::Direction::Right };
		glm::bvec2 m_LockedAxis{ false, false };

		bool m_IsDirty{ true };
		minigin::RenderComponent* m_RenderComp{ nullptr };

		static const std::unordered_map<minigin::Direction, glm::ivec2> m_ToVecMap;
	};
}