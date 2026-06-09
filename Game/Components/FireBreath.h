#pragma once
#include <Component.h>
#include <Subject.h>

namespace digdug
{
	class Enemy;
	class FireBreath final : public minigin::Component
	{
	public:
		using minigin::Component::Component;

		void Start() override;
		void Update(float deltaTime) override;
		void SetCaster(Enemy* enemy) { m_Enemy = enemy; }

	private:
		minigin::Subject m_OnAttackEnded{};
		Enemy* m_Enemy{ nullptr };

		float m_LifeTime{ 2.f };
	};
}