#pragma once
#include <Component.h>
#include <string>
#include <vector>

namespace digdug
{
	class ScoreBoard final : public minigin::Component
	{
	public:
		using minigin::Component::Component;

		void Update(float) override;
		void Render() const override;

		void LoadScores(const std::string& file);
		void SaveScores();

		void AddScore(const std::string& name, int score);

	private:
		struct SavedScore
		{
			std::string name{};
			int score{};
		};

		std::vector<SavedScore> m_Scores{};
		std::string m_File{};

		bool m_BoardChanged{ false };

		const std::string m_NameSyntax{ "name" };
		const std::string m_ScoreSyntax{ "score" };
	};
}