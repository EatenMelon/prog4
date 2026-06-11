#include "SelectorCmds.h"
#include <ObjectSelector.h>

digdug::SelectCmd::SelectCmd(ObjectSelector* selector, float delay)
	: m_Selector{ selector }
	, m_Delay{ delay }
{
}

void digdug::SelectCmd::Execute(const minigin::InputContext& context, float deltaTime)
{
	if (m_Selector == nullptr) return;

	if (context.frame == m_LastFrame) return;
	m_LastFrame = context.frame;

	m_TimeUntilSelect -= deltaTime;
	if (m_TimeUntilSelect > 0.f) return;
	m_TimeUntilSelect = m_Delay;

	if (context.axis.y > 0)
	{
		m_Selector->SelectForwards();
	}

	if (context.axis.y < 0)
	{
		m_Selector->SelectBackwards();
	}

}

digdug::SubmitCmd::SubmitCmd(ObjectSelector* selector)
	: m_Selector{ selector }
{
}

void digdug::SubmitCmd::Execute(const minigin::InputContext&, float)
{
	if (m_Selector == nullptr) return;

	m_Selector->Submit();
}
