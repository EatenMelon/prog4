#include "SkipLevelCmd.h"
#include <GameManager.h>

void digdug::SkipLevelCmd::Execute(const minigin::InputContext&, float)
{
	GameManager::GetInstance().SkipLevel();
}