#pragma once
#include <memory>
#include <glm/glm.hpp>

namespace digdug
{
	class Enemy;
	class DirtGrid;

	//
	// 0. wandering
	// 1. frozen
	// 2. ghost
	// 
	// 0 => 1	when starting attack
	// 0 => 1	on inflated
	// 
	// 1 => 0	at end of attack
	// 1 => 0	on deflated
	// 
	// 1 => 2	randomized choice
	// 2 => 0	entered a new tunnel
	//
	class EnemyBehaviorState
	{
	public:
		EnemyBehaviorState(Enemy* enemy, DirtGrid* grid)
			: m_Enemy{ enemy }
			, m_Grid{ grid }
		{ }
		virtual ~EnemyBehaviorState() = default;

		virtual std::unique_ptr<EnemyBehaviorState> Update(float deltaTime) = 0;
		virtual std::unique_ptr<EnemyBehaviorState> OnInflatedEnter() = 0;
		virtual std::unique_ptr<EnemyBehaviorState> OnDeflatedEnter() = 0;

	protected:
		glm::ivec2 GetEnemyGridPos();

		Enemy* GetEnemy() { return m_Enemy; }
		DirtGrid* GetGrid() { return m_Grid; }

	private:
		Enemy* m_Enemy{ nullptr };
		DirtGrid* m_Grid{ nullptr };
	};

	class EnemyRoamingState final : public EnemyBehaviorState
	{
	public:
		EnemyRoamingState(Enemy* enemy, DirtGrid* grid);

		std::unique_ptr<EnemyBehaviorState> Update(float deltaTime) override;
		std::unique_ptr<EnemyBehaviorState> OnInflatedEnter() override;
		std::unique_ptr<EnemyBehaviorState> OnDeflatedEnter() override { return nullptr; }

	private:
		void SelectTarget();

		glm::ivec2 m_CurrentGridPos{};
		glm::ivec2 m_TargetGridPos{};

	};
}