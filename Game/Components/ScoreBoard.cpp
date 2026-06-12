#include "ScoreBoard.h"
#include <fstream>
#include <algorithm>

#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

#include <ResourceManager.h>
#include <Renderer.h>
#include <GameObject.h>

using Json = nlohmann::json;

void digdug::ScoreBoard::Update(float)
{
	if (!m_BoardChanged) return;

	std::sort
	(
		m_Scores.begin(),
		m_Scores.end(),
		[](const SavedScore& a, const SavedScore& b)
		{
			return a.score > b.score;
		}
	);

	m_BoardChanged = false;
}

void digdug::ScoreBoard::Render() const
{
	auto renderer{ minigin::Renderer::GetInstance().GetSDLRenderer() };

	const size_t maxVisibleScores{ 10 };

	const float scale{ 2.f };
	const float gaps{ 25.f };

	auto origin = GetOwner().GetWorldPosition();

	SDL_SetRenderScale(renderer, scale, scale);

	for (size_t s{ 0 }; s < maxVisibleScores; ++s)
	{
		glm::vec2 pos = origin;
		pos.y += gaps * s;
		pos /= scale;

		std::string text{"--- : ---"};

		if (s < m_Scores.size() && !m_Scores.empty())
		{
			auto& score = m_Scores[s];
			text = score.name + " : " + std::to_string(score.score);
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDebugText(renderer, pos.x, pos.y, text.c_str());
	}

	SDL_SetRenderScale(renderer, 1.f, 1.f);
}

void digdug::ScoreBoard::LoadScores(const std::string& file)
{
	m_Scores.clear();

	m_File = file;
	const auto path{ minigin::ResourceManager::GetInstance().GetDataPath() / file };

	std::ifstream fs{ path.string() };

	if (fs.fail()) return;

	Json jsonFile = Json::parse(fs);

	for (auto& obj : jsonFile)
	{
		auto& nameObj = obj[m_NameSyntax];
		auto& scoreObj = obj[m_ScoreSyntax];

		if (!nameObj.is_string()) return;
		if (!scoreObj.is_number_integer()) return;

		const std::string name{ nameObj };
		const int score{ scoreObj };

		AddScore(name, score);
	}

	m_BoardChanged = true;
}

void digdug::ScoreBoard::SaveScores()
{
	Json jsonFile = Json::array();

	for (const auto& score : m_Scores)
	{
		Json obj{};
		obj[m_NameSyntax] = score.name;
		obj[m_ScoreSyntax] = score.score;

		jsonFile.push_back(obj);
	}

	const auto path = minigin::ResourceManager::GetInstance().GetDataPath() / m_File;

	std::ofstream fs{ path.string() };

	if (!fs.is_open()) return;

	fs << jsonFile.dump(4);
}

void digdug::ScoreBoard::AddScore(const std::string& name, int score)
{
	const auto itr = std::find_if(m_Scores.begin(), m_Scores.end(), [name](const SavedScore& s) { return s.name == name; });

	if (itr == m_Scores.end())
	{
		SavedScore newScore{};
		newScore.name = name;
		newScore.score = score;

		m_Scores.push_back(newScore);
		return;
	}

	if (score < itr->score) return;

	itr->score = score;
	m_BoardChanged = true;
}
