#include "LetterSelectCmd.h"

#include <GameObject.h>
#include <TextComponent.h>
#include <ObjectSelector.h>
#include <Button.h>

digdug::LetterSelectCmd::LetterSelectCmd(ObjectSelector* selector, float delay)
	: m_Selector{ selector }
	, m_Delay{ delay }
{
}

void digdug::LetterSelectCmd::Execute(const minigin::InputContext& context, float deltaTime)
{
	if (context.frame == m_LastFrame) return;
	m_LastFrame = context.frame;

	m_TimeUntilSelect -= deltaTime;
	if (m_TimeUntilSelect > 0.f) return;
	m_TimeUntilSelect = m_Delay;

	auto obj = m_Selector->GetSelectedObject();

	auto button = obj->GetComponent<digdug::Button>();
	if (button != nullptr) return;

	auto textComp = obj->GetComponent<minigin::TextComponent>();
	if (textComp == nullptr) return;

	const std::string original{ textComp->GetText() };

	char letter{ 'A' };

	if (!original.empty())
	{
		letter = original[0];
	}

	if (context.axis.y > 0)
	{
		++letter;
	}
	else if (context.axis.y < 0)
	{
		--letter;
	}

	if (letter < m_StartChar)
	{
		letter = m_EndChar;
	}
	else if (letter > m_EndChar)
	{
		letter = m_StartChar;
	}

	const std::string result{ letter };
	textComp->SetText(result);
}
