#include "Button.h"
#include <ObjectSelector.h>

void digdug::Button::Start()
{
	SubmitEvent event{};
	m_SubmitEventHash = event.GetEventHash();
}

void digdug::Button::OnNotify(const minigin::IEvent& event)
{
	if (event.GetEventHash() != m_SubmitEventHash) return;

	m_Function();
}

void digdug::Button::SetSubmitFunction(const std::function<void()>& f)
{
	m_Function = f;
}
