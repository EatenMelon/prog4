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
		virtual ~ActorCommand() { m_Actor = nullptr; }

	protected:
		GameObject& GetActor() const { return *m_Actor; }

	private:
		GameObject* m_Actor;

	};

	class CharacterController;
	class MoveCommand final : public ActorCommand
	{
	public:
		MoveCommand(GameObject* pActor, float movementSpeed);

		MoveCommand(const MoveCommand& other) = delete;
		MoveCommand& operator=(const MoveCommand& other) = delete;
		MoveCommand(MoveCommand&& other) = delete;
		MoveCommand& operator=(MoveCommand&& other) = delete;

		void Execute(const InputContext& context, float deltaTime) override;

	private:
		float m_MovementSpeed;
		

	};
}