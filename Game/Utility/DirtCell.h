#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <limits>

namespace minigin
{
	class Texture2D;
}

namespace digdug
{
	class DirtCell
	{
	public:
		DirtCell(minigin::Texture2D& tiles, int rowFull = 0, int rowSide = 1, int rowCorner = 2);
		void Render(const glm::vec2& pos, float cellSize) const;

		void Dig(const glm::ivec2& entryPoint);
		void Refill() { m_SubCells = std::numeric_limits<uint8_t>::max(); }
		bool HasBeenDug() const { return !GetSubCell(1, 1); }

		enum class Side{ Top, Bottom, Left, Right };
		bool GetSide(Side side) const;

	private:
		void DrawSubCell(const glm::vec2& pos, float subCellSize, int x, int y) const;
		bool IsCorner(int x, int y) const;
		bool HasHorizontalNeightbours(int x, int y) const;
		bool HasHVerticalNeightbours(int x, int y) const;

		bool GetSubCell(int x, int y) const;
		void SetSubCell(int x, int y, bool value);
		float GetSubCellSpriteRotation(int x, int y) const;

		minigin::Texture2D* m_TileSheet{ nullptr };
		const int m_RowFullTile{};
		const int m_RowSideTile{};
		const int m_RowCornerTile{};
		
		uint8_t m_SubCells{ std::numeric_limits<uint8_t>::max() };

		static constexpr uint8_t m_Size{ 3 };
		static constexpr int m_BitMap[m_Size * m_Size]
		{
			0, 1, 2,
			3, -1, 4,
			5, 6, 7
		};
	};
}