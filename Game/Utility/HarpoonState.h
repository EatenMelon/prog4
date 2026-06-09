#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <algorithm>
#include <unordered_map>

#include <MiniginTypes.h>
#include <Events.h>
#include <Observer.h>
#include <Subject.h>

namespace minigin
{
	class GameObject;
	class RenderComponent;
}

namespace digdug
{
	class Harpoon;
	class Inflatable;
	class AimComponent;
	class DirtGrid;

	//
	// 0. do nothing
	// 1. shoot harpoon
	// 2. pump enemy
	// 3. retrackt
	// 
	// 0 => 1	with function for commands
	// 
	// 1 => 2	if harpoon hits enemy
	// 
	// 1 => 3	if harpoon doesn't hit enemy
	// 2 => 3	if enemy exploded
	// 2 => 3	force cancel
	// 
	// 3 => 0	fully retracted
	//
	class HarpoonState
	{
	public:
		HarpoonState(Harpoon* harpoon, float extend = 0.f);
		virtual ~HarpoonState() = default;

		virtual std::unique_ptr<HarpoonState> Update(float, DirtGrid*) = 0;
		virtual std::unique_ptr<HarpoonState> StartShoot() = 0;
		virtual std::unique_ptr<HarpoonState> StartRetract() = 0;
		virtual std::unique_ptr<HarpoonState> OnAttach(Inflatable*) = 0;

		virtual bool UsingHarpoon() { return false; }

		float GetExtend() const { return std::clamp(m_Extended, 0.f, 1.f); };
		glm::vec2 GetHarpoonSize() const;

	protected:
		Harpoon* GetHarpoon() const { return m_Harpoon; }

		void Extend(float percent) { m_Extended += percent; }
		void Retract(float percent) { m_Extended -= percent; }

		void UpdatePosition();
		void UpdateHitbox();

	private:
		Harpoon* m_Harpoon{ nullptr };
		float m_Extended{ 0.f };
	};

	// Do nothing, wait until harpoon is launched.
	class HarpoonIdleState final : public HarpoonState
	{
	public:
		HarpoonIdleState(Harpoon* harpoon);

		std::unique_ptr<HarpoonState> StartShoot() override;

		std::unique_ptr<HarpoonState> Update(float, DirtGrid*) override { return nullptr; }
		std::unique_ptr<HarpoonState> StartRetract() override { return nullptr; }
		std::unique_ptr<HarpoonState> OnAttach(Inflatable*) override { return nullptr; }
	};

	// Launch harpoon forward until it hits an enemy, or until it's fully extended.
	class HarpoonShootState final : public HarpoonState
	{
	public:
		HarpoonShootState(Harpoon* harpoon);

		bool UsingHarpoon() override { return true; }

		std::unique_ptr<HarpoonState> Update(float deltaTime, DirtGrid*) override;
		std::unique_ptr<HarpoonState> OnAttach(Inflatable* inflatable) override;
		std::unique_ptr<HarpoonState> StartRetract() override;

		std::unique_ptr<HarpoonState> StartShoot() override { return nullptr; }

	private:
		const float m_LaunchSpeed{ 0.85f };
	};

	// event for pumping inflatables
	class PumpInflatableEvent final : public minigin::PlainEvent
	{
	public:
		PumpInflatableEvent(minigin::GameObject* pumpUser)
			: minigin::PlainEvent("PumpEvent")
			, m_PumpUser{ pumpUser }
		{}

		minigin::GameObject* GetPumpUser() const { return m_PumpUser; }

	private:
		minigin::GameObject* m_PumpUser{ nullptr };
	};

	class PumpDetachEvent final : public minigin::PlainEvent
	{
	public:
		PumpDetachEvent() : minigin::PlainEvent("DetachEvent") {}
	};

	// Pump inflatable until it pops
	class HarpoonPumpingState final : public HarpoonState, public minigin::IObserver
	{
	public:
		HarpoonPumpingState(Harpoon* harpoon, float extend, Inflatable* inflatable);
		~HarpoonPumpingState() override;

		bool UsingHarpoon() override { return true; }

		std::unique_ptr<HarpoonState> Update(float deltaTime, DirtGrid*) override;
		void OnNotify(const minigin::IEvent& event) override;
		std::unique_ptr<HarpoonState> StartShoot() override;
		std::unique_ptr<HarpoonState> StartRetract() override;

		std::unique_ptr<HarpoonState> OnAttach(Inflatable*) override { return nullptr; };

	private:
		Inflatable* m_Inflatable{ nullptr };

		const float m_PumpDelay{ 1.f };
		float m_TimeUntilNextPump{ 0.f };
		minigin::Subject m_PumpEvent{};

		bool m_TryRetract{ false };
		const float m_RetractDelay{ 0.25f };
		float m_TimeUntilRetract{ 0.f };

		bool m_RetractNextFrame{ false };
		unsigned int m_PoppedEventHash{ 0 };
	};

	// Retract harpoon until fully closed.
	class HarpoonRetractState final : public HarpoonState
	{
	public:
		HarpoonRetractState(Harpoon* harpoon, float extend);

		std::unique_ptr<HarpoonState> Update(float deltaTime, DirtGrid*) override;

		std::unique_ptr<HarpoonState> StartShoot() override { return nullptr; }
		std::unique_ptr<HarpoonState> StartRetract() override { return nullptr; }
		std::unique_ptr<HarpoonState> OnAttach(Inflatable*) override { return nullptr; }

	private:
		const float m_RetractSpeed{ 0.85f };
	};
}