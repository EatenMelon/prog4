#include "HarpoonCmd.h"
#include <Harpoon.h>

digdug::HarpoonCmd::HarpoonCmd(int playerID, Harpoon* harpoon, const std::function<void(Harpoon*)>& callback)
	: ActorCommand(harpoon->GetUser(), playerID)
	, m_Harpoon{ harpoon }
	, m_Callback{ callback }
{
}

void digdug::HarpoonCmd::ActorExecute(const minigin::InputContext&, float)
{
	if (m_Harpoon == nullptr)
	{
		std::cerr << "ERROR: HarpoonCmd::Execute, cannot launch harpoon since m_Harpoon was nullptr!\n";
		return;
	}

	if (!m_Harpoon->Enabled()) return;

	m_Callback(m_Harpoon);
}
