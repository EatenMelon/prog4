#pragma once
#include <Component.h>
#include <DirtCell.h>
#include <Texture2D.h>
#include <glm/glm.hpp>

#include <vector>
#include <queue>
#include <unordered_map>

namespace digdug
{
	class DirtGrid final : public minigin::Component
	{
	public:
		using::minigin::Component::Component;

		enum class Depth{ Surface, TopSoil, SubSoil, Stone, Bedrock };

		void Start() override;
		void Update(float) override;
		void Render() const override;
		void GuiRender() override;

		void Dig(const glm::ivec2& start, const glm::ivec2& end, const char preferredAxis);

		void SetTileTexture(Depth depth, minigin::Texture2D& texture);
		void SetCellSize(float size) { m_CellSize = size; }

		Depth GetDepthLevel(const glm::ivec2& pos) const;

		glm::vec2 GetSize() const { return glm::vec2{ m_Width * m_CellSize, m_Height * m_CellSize }; }

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
		const int m_Height{ 15 };

		std::vector<DirtCell> m_Cells{};
		std::queue<DigEvent> m_DiggingQueue{};
		std::unordered_map<Depth, minigin::Texture2D*> m_TileTextures{};
	};
}