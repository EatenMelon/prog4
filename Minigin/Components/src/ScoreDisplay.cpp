#include "ScoreDisplay.h"

#include <sstream>

#include "TextComponent.h"
#include "ScoreComponent.h"
#include "GameObject.h"

void dae::ScoreDisplay::Init()
{
	UpdateDisplay();
}

void dae::ScoreDisplay::OnNotify(Event event)
{
	if (event != Event::ReceivedPoints) return;

	UpdateDisplay();
}

void dae::ScoreDisplay::UpdateDisplay()
{
	if (m_Display == nullptr)
	{
		auto txtComp = GetOwner().GetComponent<TextComponent>();

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
