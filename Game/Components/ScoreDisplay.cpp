#include "ScoreDisplay.h"

#include <sstream>

#include "TextComponent.h"
#include "ScoreComponent.h"
#include "GameObject.h"

void digdug::ScoreDisplay::Init()
{
	UpdateDisplay();
}

void digdug::ScoreDisplay::OnNotify(const minigin::IEvent* event)
{
	auto plain = static_cast<const minigin::PlainEvent*>(event);

	if (plain->GetEventHash() != minigin::PlainEvent::CreateHashSDBM("points")) return;

	UpdateDisplay();
}

void digdug::ScoreDisplay::UpdateDisplay()
{
	if (m_Display == nullptr)
	{
		auto txtComp = GetOwner().GetComponent<minigin::TextComponent>();

		if (txtComp == nullptr) return;

		m_Display = txtComp;
	}

	if (m_Score == nullptr)
	{
		auto scoreComp = GetOwner().GetComponent<ScoreComponent>();

		if (scoreComp == nullptr) return;

		m_Score = scoreComp;
	}

	std::stringstream ss{};
	ss << "Score: " << m_Score->GetScore();

	m_Display->SetText(ss.str());
}
