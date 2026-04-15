#pragma once
#include <glm/glm.hpp>

namespace dae
{
	struct InputContext
	{
		glm::vec2 axis{};
	};

	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Execute(const InputContext& context, float deltaTime) = 0;

	private:

	};

	class GameObject;
	class ActorCommand : public Command
	{
	public:
		ActorCommand(GameObject* pActor) : m_Actor{ pActor } {};

		ActorCommand(const ActorCommand& other) = delete;
		ActorCommand& operator=(const ActorCommand& other) = delete;
		ActorCommand(ActorCommand&& other) = delete;
		ActorCommand& operator=(ActorCommand&& other) = delete;

		virtual ~ActorCommand() { m_Actor = nullptr; }

	protected:
		GameObject& GetActor() const { return *m_Actor; }

	private:
		GameObject* m_Actor;

	};

	class MoveCommand final : public ActorCommand
	{
	public:
		MoveCommand(GameObject* pActor, float movementSpeed);

		void Execute(const InputContext& context, float deltaTime) override;

	private:
		float m_MovementSpeed;
	};

	class HealthComponent;
	class DamageCommand final : public ActorCommand
	{
	public:
		DamageCommand(GameObject* pActor, HealthComponent* pHealth);

		void Execute(const InputContext& context, float deltaTime) override;
	
	private:
		HealthComponent* m_pHealth;

	};

	class ScoreComponent;
	class ScoreCommand final : public ActorCommand
	{
	public:
		ScoreCommand(GameObject* pActor, ScoreComponent* pScore);

		void Execute(const InputContext& context, float deltaTime) override;

	private:
		ScoreComponent* m_pScore;

	};
}