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
		virtual void Execute(const InputContext& context) = 0;

	private:

	};

	class CharacterController;
	class MoveCommand final : public Command
	{
	public:
		MoveCommand(CharacterController* pController);

		MoveCommand(const MoveCommand& other) = delete;
		MoveCommand& operator=(const MoveCommand& other) = delete;
		MoveCommand(MoveCommand&& other) = delete;
		MoveCommand& operator=(MoveCommand&& other) = delete;

		~MoveCommand() override;

		void Execute(const InputContext& context) override;

	private:
		CharacterController* m_pController;
	};
}