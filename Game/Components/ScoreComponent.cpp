#include "ScoreComponent.h"

#include <sstream>
#include <iostream>

#include <TextComponent.h>
#include <Events.h>

void digdug::ScoreComponent::Start()
{
	minigin::HitEvent event{ nullptr };

	UpdateDisplay();
}

void digdug::ScoreComponent::LinkTextComponent(minigin::TextComponent* comp, const std::string& message)
{
	if (comp == nullptr) return;

	m_Display = comp;
	m_Message = message;

	UpdateDisplay();
}

void digdug::ScoreComponent::SetScore(int score)
{
	m_Score = score;
	UpdateDisplay();	
}

void digdug::ScoreComponent::AddScore(int addedScore)
{
	m_Score += addedScore;
	UpdateDisplay();
}

void digdug::ScoreComponent::UpdateDisplay()
{
	if (m_Display == nullptr)
	{
		return;
	}

	try
	{
		std::stringstream ss{};
		ss << m_Message << m_Score;

		m_Display->SetText(ss.str());
	}
	catch (const std::exception& e)
	{
		std::cerr << m_Message << e.what() << "\n";
	}

}
