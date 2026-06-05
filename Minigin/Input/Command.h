#pragma once
#include <glm/glm.hpp>
#include <SceneManager.h>

namespace minigin
{
	struct InputContext
	{
		glm::vec2 axis{};
		int playerID{ -1 };
		int frame{ 0 };
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
		ActorCommand(GameObject* pActor, int playerID)
			: m_Actor{ pActor }, m_playerID{ playerID }
		{}

		ActorCommand(const ActorCommand& other) = delete;
		ActorCommand& operator=(const ActorCommand& other) = delete;
		ActorCommand(ActorCommand&& other) = delete;
		ActorCommand& operator=(ActorCommand&& other) = delete;

		void Execute(const InputContext& context, float deltaTime) override final
		{
			if (SceneManager::GetInstance().ActiveSceneId() != m_Actor->GetSceneId())
			{
				return;
			}

			if (context.playerID != m_playerID)
			{
				return;
			}

			ActorExecute(context, deltaTime);
		}

		virtual ~ActorCommand() = default;

	protected:
		GameObject& GetActor() const { return *m_Actor; }
		virtual void ActorExecute(const InputContext& context, float deltaTime) = 0;

	private:
		GameObject* m_Actor;
		int m_playerID{};
	};



	// standard move command, so i'll keep it in the engine
	class MoveCommand final : public ActorCommand
	{
	public:
		MoveCommand(GameObject* pActor, int playerID, float movementSpeed);

	protected:
		void ActorExecute(const InputContext& context, float deltaTime) override;

	private:
		float m_MovementSpeed;
	};
}