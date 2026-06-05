#include "Command.h"
#include "GameObject.h"

minigin::MoveCommand::MoveCommand(GameObject* pActor, int playerID, float movementSpeed)
	: ActorCommand(pActor, playerID)
	, m_MovementSpeed{ movementSpeed }
{

}

void minigin::MoveCommand::ActorExecute(const InputContext& context, float deltaTime)
{
	glm::vec3 pos = GetActor().GetLocalPosition();

	pos.x += context.axis.x * m_MovementSpeed * deltaTime;
	pos.y += context.axis.y * m_MovementSpeed * deltaTime;

	GetActor().SetLocalPosition(pos);
}

