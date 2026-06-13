#pragma once
#include <Component.h>
#include <DirtCell.h>
#include <Texture2D.h>
#include <glm/glm.hpp>

#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>

namespace digdug
{
	class DirtGrid final : public minigin::Component
	{
	public:
		using::minigin::Component::Component;

		enum class Depth{ Surface = 0, TopSoil = 1, SubSoil = 2, Stone = 3, Bedrock = 4 };

		void Start() override;
		void Update(float) override;
		void Render() const override;

		void Dig(const glm::ivec2& start, const glm::ivec2& end, const char preferredAxis);
		bool HasBeenDug(const glm::ivec2& gridPos) const;

		// returns true:
		// * if start and end are directly connected (non-diagonal),
		// * assuming both cells have been dug
		bool IsWallOpen(const glm::ivec2& start, const glm::ivec2& end) const;

		glm::vec3 GetCellLocalPos(const glm::ivec2& gridPos) const;
		glm::ivec2 GetPosInGrid(const glm::vec3& pos) const;

		void SetTileTexture(Depth depth, std::shared_ptr<minigin::Texture2D> texture);
		void SetCellSize(float size) { m_CellSize = size; }

		Depth GetDepthLevel(const glm::ivec2& pos) const;

		glm::vec2 GetSize() const { return glm::vec2{ m_Width * m_CellSize, m_Height * m_CellSize }; }
		float GetCellSize() const { return m_CellSize; }

	private:
		struct DigEvent
		{
			glm::ivec2 start{};
			glm::ivec2 end{};
			char preferredAxis{};
		};

		void DigTunnel(const DigEvent& digEvent);
		void DigOneWay(int start, int end, int oppositeAxis, const char axis);

		float m_CellSize{ 1.f };

		const int m_Width{ 14 };
		const int m_Height{ 17 };

		bool m_ShowOutlines{ false };

		std::vector<DirtCell> m_Cells{};
		std::queue<DigEvent> m_DiggingQueue{};
		std::unordered_map<Depth, std::shared_ptr<minigin::Texture2D>> m_TileTextures{};
	};
}