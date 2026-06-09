#pragma once
#include <memory>
#include <string>
#include <Component.h>
#include <Subject.h>
#include <Events.h>
#include <glm/glm.hpp>
#include <vector>

namespace minigin
{
	class Texture2D;
	class RenderComponent;
	class GameObject;
}

namespace digdug
{
	class EnemyInflatedEvent final : public minigin::PlainEvent
	{
	public:
		EnemyInflatedEvent(minigin::GameObject* pumpUser)
			: minigin::PlainEvent("EnemyInflated")
			, m_PumpUser{ pumpUser }
		{}

		minigin::GameObject* GetPumpUser() const { return m_PumpUser; }

	private:
		minigin::GameObject* m_PumpUser{ nullptr };
	};

	class EnemyDeflatedEvent final : public minigin::PlainEvent
	{
	public:
		EnemyDeflatedEvent() : minigin::PlainEvent("EnemyDeflated") {}
	};

	class Attack;
	class Inflatable;
	class AimComponent;
	class Enemy final : public minigin::Component
	{
	public:
		using minigin::Component::Component;

		void Start() override;
		void Update(float) override;

		void BecomeDefault();
		void BecomeGhost();

		void SetDefaultSprite(const std::string& path);
		void SetGhostSprite(const std::string& path);

		void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
		float GetMovementSpeed() const { return m_MovementSpeed; }

		minigin::Subject& OnInflatedEnter() { return m_OnInflatedEnter; }
		minigin::Subject& OnDeflatedEnter() { return m_OnDeflatedEnter; }

		glm::vec2 GetSize();
		AimComponent* GetAimComponent() { return m_AimComponent; }

		void AddPossibleTarget(minigin::GameObject* obj);
		minigin::GameObject* GetRandomTarget();

	private:
		minigin::RenderComponent* m_RenderComp{ nullptr };
		AimComponent* m_AimComponent{ nullptr };

		std::shared_ptr<minigin::Texture2D> m_DefaultSprite{ nullptr };
		std::shared_ptr<minigin::Texture2D> m_GhostSprite{ nullptr };

		Inflatable* m_Inflatable{ nullptr };
		bool m_WasInflated{ false };
		minigin::Subject m_OnInflatedEnter{};
		minigin::Subject m_OnDeflatedEnter{};

		float m_MovementSpeed{ 100.f };

		std::vector<minigin::GameObject*> m_PossibleTargets{};
	};
}