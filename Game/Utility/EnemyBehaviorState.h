#pragma once
#include <memory>
#include <glm/glm.hpp>

namespace minigin
{
	class GameObject;
}

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
	// 0 => 1	randomized choice => attack
	// 0 => 2	randomized choice => ghost
	// 
	// 1 => 0	at end of attack
	// 1 => 0	on deflated
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
		virtual std::unique_ptr<EnemyBehaviorState> OnInflatedEnter(minigin::GameObject* pumpUser) = 0;
		virtual std::unique_ptr<EnemyBehaviorState> OnDeflatedEnter() = 0;
		virtual std::unique_ptr<EnemyBehaviorState> OnAttackEnded() = 0;

	protected:
		glm::ivec2 GetEnemyGridPos();

		Enemy* GetEnemy() { return m_Enemy; }
		DirtGrid* GetGrid() { return m_Grid; }

	private:
		Enemy* m_Enemy{ nullptr };
		DirtGrid* m_Grid{ nullptr };
	};

	// wander around
	class EnemyWanderState final : public EnemyBehaviorState
	{
	public:
		EnemyWanderState(Enemy* enemy, DirtGrid* grid);

		std::unique_ptr<EnemyBehaviorState> Update(float deltaTime) override;
		std::unique_ptr<EnemyBehaviorState> OnInflatedEnter(minigin::GameObject*) override;
		std::unique_ptr<EnemyBehaviorState> OnDeflatedEnter() override { return nullptr; }
		std::unique_ptr<EnemyBehaviorState> OnAttackEnded() override { return nullptr; }

	private:
		void SelectTarget();
		bool CanAttack();

		glm::ivec2 m_CurrentGridPos{};
		glm::ivec2 m_TargetGridPos{};

		float m_TimeUntilNewAction{ 0.f };
		const float m_TryNewActionDelay{ 5.f };

		const int m_NumActions{ 3 };
		enum class NewAction
		{
			None = 0,
			TurnGhost = 1,
			Attack = 2
		};
	};

	// do nothing
	class EnemyFrozenState final : public EnemyBehaviorState
	{
	public:
		EnemyFrozenState(Enemy* enemy, DirtGrid* grid);

		std::unique_ptr<EnemyBehaviorState> OnDeflatedEnter() override;
		std::unique_ptr<EnemyBehaviorState> Update(float) override { return nullptr; }
		std::unique_ptr<EnemyBehaviorState> OnInflatedEnter(minigin::GameObject*) override { return nullptr; }
		std::unique_ptr<EnemyBehaviorState> OnAttackEnded() override { return nullptr; };
	};

	// also do nothing
	class EnemyAttackingState final : public EnemyBehaviorState
	{
	public:
		EnemyAttackingState(Enemy* enemy, DirtGrid* grid);

		std::unique_ptr<EnemyBehaviorState> OnAttackEnded() override;
		std::unique_ptr<EnemyBehaviorState> OnInflatedEnter(minigin::GameObject*) override;
		std::unique_ptr<EnemyBehaviorState> OnDeflatedEnter() override { return nullptr; };
		std::unique_ptr<EnemyBehaviorState> Update(float) override { return nullptr; }
	};

	class EnemyGhostState final : public EnemyBehaviorState
	{
	public:
		EnemyGhostState(Enemy* enemy, DirtGrid* grid);

		std::unique_ptr<EnemyBehaviorState> Update(float deltaTime) override;
		std::unique_ptr<EnemyBehaviorState> OnInflatedEnter(minigin::GameObject* pumpUser) override;
		std::unique_ptr<EnemyBehaviorState> OnDeflatedEnter() override { return nullptr; }
		std::unique_ptr<EnemyBehaviorState> OnAttackEnded() override { return nullptr; }

	private:
		void ChangeDirection();
		bool EneteredNewTunnel();

		glm::ivec2 m_CellOfTransformation{ 0, 0 };

		minigin::GameObject* m_Target{ nullptr };
		glm::vec2 m_TargetSize{ 0.f, 0.f };

		const float m_RedirectionDelay{ 2.f };
		float m_TimeUntilRedirect{ 0.f };
		glm::vec2 m_Direction{ 0.f };
	};
	
}