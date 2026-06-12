#include "SelectorCmds.h"
#include <ObjectSelector.h>

digdug::SelectCmd::SelectCmd(ObjectSelector* selector, char axis, float delay)
	: m_Selector{ selector }
	, m_Delay{ delay }
	, m_Axis{ axis }
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

	constexpr float min{ 0.5f };

	float axis{ 0.f };

	switch (m_Axis)
	{
	case 'x':
	case 'X':
		axis = context.axis.x;
		break;

	default:
		axis = context.axis.y;
		break;
	}

	if (axis > min)
	{
		m_Selector->SelectForwards();
	}

	if (axis < -min)
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
