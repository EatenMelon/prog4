#include "Command.h"
#include "CharacterController.h"

dae::MoveCommand::MoveCommand(CharacterController* pController)
	: m_pController{ pController }
{

}

dae::MoveCommand::~MoveCommand()
{
	m_pController = nullptr;
}

void dae::MoveCommand::Execute(const InputContext& context)
{
	m_pController->OnMove(context.axis);
}
