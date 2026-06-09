#pragma once
#include <Subject.h>
#include <Events.h>

namespace digdug
{
	class AttackEndedEvent final : public minigin::PlainEvent
	{
	public:
		AttackEndedEvent() : minigin::PlainEvent("AttackEnded") {}
	};

	class Enemy;
	class Attack
	{
	public:
		virtual ~Attack() = default;

		virtual void Execute(Enemy* attacker) = 0;
	};

	class FireAttack final : public Attack
	{
	public:
		FireAttack() = default;

		void Execute(Enemy* attacker) override;
	};
}
