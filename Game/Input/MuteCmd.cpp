#include "MuteCmd.h"
#include <ServiceLocator.h>

void digdug::MuteCmd::Execute(const minigin::InputContext&, float)
{
	const bool muted = minigin::ServiceLocator::GetSoundSystem()->IsMuted();
	minigin::ServiceLocator::GetSoundSystem()->Mute(!muted);
}
