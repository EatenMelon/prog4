#pragma once
#include <memory>
#include <Component.h>
#include <Observer.h>
#include <glm/glm.hpp>
#include <EnemyBehaviorState.h>

namespace digdug
{
	class Enemy;
	class DirtGrid;
	class EnemyBehaviorState;
	class EnemyBehavior final : public minigin::Component, public minigin::IObserver
	{
	public:
		using minigin::Component::Component;

		void Start() override;
		void Update(float deltaTime) override;

		void OnNotify(const minigin::IEvent& event) override;

		void SetGrid(DirtGrid* grid) { m_Grid = grid; }
		void SetPositionInGrid(const glm::ivec2& pos);

	private:
		std::unique_ptr<EnemyBehaviorState> m_State{ nullptr };

		Enemy* m_Enemy{ nullptr };
		DirtGrid* m_Grid{ nullptr };

		unsigned int m_InflatedEnetrHash{ 0 };
		unsigned int m_DeflatedEnetrHash{ 0 };
		unsigned int m_AttackEndedHash{ 0 };
	};
}