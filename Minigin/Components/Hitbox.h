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
		~Hitbox() override;

		void Start() override;
		void Render() const override;

		void SetSize(float width, float height);
		bool IsColliding(const Hitbox& other) const;

		Subject& HitEnterEvent() { return m_OnHitEnter; }
		Subject& HitStayEvent() { return m_OnHitStay; }
		Subject& HitExitEvent() { return m_OnHitExit; }

	private:
		friend class HitboxManager;
		void CheckCollision(Hitbox& other);

		float m_Width{ 1 };
		float m_Height{ 1 };

		std::vector<Hitbox*> m_CurrentlyColliding{};
		Subject m_OnHitEnter{};
		Subject m_OnHitStay{};
		Subject m_OnHitExit{};
	};
}