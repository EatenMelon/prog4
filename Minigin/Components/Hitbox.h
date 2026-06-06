#pragma once
#include "Component.h"
#include <vector>
#include <Subject.h>

namespace minigin
{
	class Hitbox : public Component
	{
	public:
		using Component::Component;

		Hitbox(const Hitbox&) = delete;
		Hitbox& operator=(const Hitbox&) = delete;
		Hitbox(Hitbox&&) = delete;
		Hitbox& operator=(Hitbox&&) = delete;

		~Hitbox() override;

		void Init() override;
		void Render() const override;

		void SetBounds(float width, float height);
		void SetShrink(float shrink) { m_Shrink = shrink; }
		bool IsColliding(const Hitbox& other) const;

		Subject& HitEnterEvent() { return m_OnHitEnter; }
		Subject& HitStayEvent() { return m_OnHitStay; }
		Subject& HitExitEvent() { return m_OnHitExit; }

	private:
		friend class HitboxManager;
		void CheckCollision(Hitbox& other);

		float m_Width{ 1 };
		float m_Height{ 1 };
		float m_Shrink{ 0.f };

		std::vector<Hitbox*> m_CurrentlyColliding{};
		Subject m_OnHitEnter{};
		Subject m_OnHitStay{};
		Subject m_OnHitExit{};
	};
}