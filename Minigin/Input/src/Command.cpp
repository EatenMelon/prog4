#include "Command.h"
#include "GameObject.h"

dae::MoveCommand::MoveCommand(GameObject* pActor, float movementSpeed)
	: ActorCommand(pActor)
	, m_MovementSpeed{ movementSpeed }
{

}

void dae::MoveCommand::Execute(const InputContext& context, float deltaTime)
{

	glm::vec3 pos = GetActor().GetLocalPosition();

	pos.x += context.axis.x * m_MovementSpeed * deltaTime;
	pos.y += context.axis.y * m_MovementSpeed * deltaTime;

	GetActor().SetLocalPosition(pos);
}
