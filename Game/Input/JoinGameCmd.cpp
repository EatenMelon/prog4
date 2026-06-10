#include "JoinGameCmd.h"
#include <GameManager.h>

void digdug::JoinGameCmd::Execute(const minigin::InputContext& context, float)
{
	GameManager::GetInstance().JoinPlayer(context.playerID);
}
