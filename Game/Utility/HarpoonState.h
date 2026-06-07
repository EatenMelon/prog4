#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <algorithm>
#include <MiniginTypes.h>
#include <unordered_map>

namespace minigin
{
	class GameObject;
	class RenderComponent;
}

namespace digdug
{
	class Harpoon;
	class AimComponent;

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

	class HarpoonState
	{
	public:
		HarpoonState(Harpoon* harpoon, float extend);

		virtual std::unique_ptr<HarpoonState> Update(float) = 0;
		virtual std::unique_ptr<HarpoonState> StartShoot() = 0;
		virtual std::unique_ptr<HarpoonState> StartRetract() = 0;

		float GetExtend() const { return std::clamp(m_Extended, 0.f, 1.f); };
		glm::vec2 GetHarpoonSize() const;

	protected:
		Harpoon* GetHarpoon() const { return m_Harpoon; }

		void Extend(float percent) { m_Extended += percent; }
		void Retract(float percent) { m_Extended -= percent; }

		void UpdatePosition();

	private:
		float m_Extended{ 0.f };
		minigin::Direction m_LastDir{ minigin::Direction::None };

		Harpoon* m_Harpoon{ nullptr };

		static std::unordered_map<minigin::Direction, glm::ivec2> m_PositionMap;
	};

	class IdleState final : public HarpoonState
	{
	public:
		using HarpoonState::HarpoonState;

		std::unique_ptr<HarpoonState> Update(float) override;
		std::unique_ptr<HarpoonState> StartShoot() override;
		std::unique_ptr<HarpoonState> StartRetract() override;

	private:
	};
}