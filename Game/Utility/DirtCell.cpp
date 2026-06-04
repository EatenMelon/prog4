#include <DirtCell.h>
#include <Texture2D.h>
#include <Renderer.h>

digdug::DirtCell::DirtCell(minigin::Texture2D& tiles, int rowFull, int rowSide, int rowCorner)
	: m_TileSheet{ &tiles }
	, m_RowFullTile{ rowFull }
	, m_RowSideTile{ rowSide }
	, m_RowCornerTile{ rowCorner }
{
}

void digdug::DirtCell::Render(const glm::vec2& pos, float cellSize) const
{
	for (uint8_t sc{ 0 }; sc < m_Size * m_Size; ++sc)
	{
		int x = sc % m_Size;
		int y = sc / m_Size;

		glm::vec2 subPos{};
		subPos.x = pos.x + x * cellSize / m_Size;
		subPos.y = pos.y + y * cellSize / m_Size;

		DrawSubCell(subPos, cellSize / m_Size, x, y);
	}
}

void digdug::DirtCell::Dig(const glm::vec2& entryPoint)
{
	const int near{ 0 };
	const int middle{ 1 };
	const int far{ 2 };

	if (entryPoint.x > 0)
	{
		SetSubCell(far, middle, false);
	}

	if (entryPoint.x < 0)
	{
		SetSubCell(near, middle, false);
	}

	if (entryPoint.y > 0)
	{
		SetSubCell(middle, far, false);
	}

	if (entryPoint.y < 0)
	{
		SetSubCell(middle, near, false);
	}
}

void digdug::DirtCell::DrawSubCell(const glm::vec2& pos, float subCellSize, int x, int y) const
{
	if (!GetSubCell(x, y)) return;
	if (m_TileSheet == nullptr) return;

	const bool horNeighbours = HasHorizontalNeightbours(x, y);
	const bool verNeihbours = HasHVerticalNeightbours(x, y);

	if (!horNeighbours && !verNeihbours) return;

	SDL_FRect dst{};
	dst.x = pos.x;
	dst.y = pos.y;
	dst.w = subCellSize;
	dst.h = subCellSize;

	SDL_FRect src{};
	src.w = m_TileSheet->GetSize().x;
	src.h = m_TileSheet->GetSize().y / m_Size;	// I expect them to be square
	src.x = 0.f;
	src.y = m_RowFullTile * src.h;

	auto renderer = minigin::Renderer::GetInstance().GetSDLRenderer();
	if (m_SubCells == std::numeric_limits<uint8_t>::max())
	{
		SDL_RenderTexture(renderer, m_TileSheet->GetSDLTexture(), &src, &dst);
		return;
	}

	int row{ m_RowSideTile };

	if (IsCorner(x, y) && horNeighbours && verNeihbours)
	{
		row = m_RowCornerTile;
	}

	const float angle{ GetSubCellSpriteRotation(x, y) };

	src.y = row * src.h;

	SDL_FPoint center{};
	center.x = subCellSize / 2;
	center.y = subCellSize / 2;
	SDL_RenderTextureRotated(renderer, m_TileSheet->GetSDLTexture(), &src, &dst, angle, &center, SDL_FLIP_NONE);
}

bool digdug::DirtCell::IsCorner(int x, int y) const
{
	if (x != 0 && x != m_Size - 1) return false;
	if (y != 0 && y != m_Size - 1) return false;

	return true;
}

bool digdug::DirtCell::HasHorizontalNeightbours(int x, int y) const
{
	return GetSubCell(x + 1, y) || GetSubCell(x - 1, y);
}

bool digdug::DirtCell::HasHVerticalNeightbours(int x, int y) const
{
	return GetSubCell(x, y + 1) || GetSubCell(x, y - 1);
}

bool digdug::DirtCell::GetSubCell(int x, int y) const
{
	if (x >= m_Size) return false;
	if (y >= m_Size) return false;
	if (x < 0) return false;
	if (y < 0) return false;

	int bit{ m_BitMap[x + y * m_Size] };

	if (bit < 0)
	{
		// center cell (marked as -1) can only be true
		// if all cells around it are true as well
		return m_SubCells == 0xFF;
	}

	return (m_SubCells & (1u << bit)) != 0;
}

void digdug::DirtCell::SetSubCell(int x, int y, bool value)
{
	if (x >= m_Size) return;
	if (y >= m_Size) return;

	int bit{ m_BitMap[x + y * m_Size] };

	if (bit < 0) return;

	uint8_t mask = (1u << bit);

	if (value)
	{
		m_SubCells |= mask;
		return;
	}
	
	m_SubCells &= ~mask;
}

float digdug::DirtCell::GetSubCellSpriteRotation(int x, int y) const
{
	const bool up = GetSubCell(x, y - 1);
	const bool down = GetSubCell(x, y + 1);
	const bool left = GetSubCell(x - 1, y);
	const bool right = GetSubCell(x + 1, y);

	if (!up && !down && !left && !right) return 0.f;

	constexpr float turnAngle = 90.f;
	int turns{ 0 };

	// for corner tiles
	if (up && left)				turns = 0;
	else if (up && right)		turns = 1;
	else if (down && right)		turns = 2;
	else if (down && left)		turns = 3;

	// for corners that "become sides"
	else if (HasHorizontalNeightbours(x, y) && y == 0)	turns = 2;
	else if (HasHorizontalNeightbours(x, y))			turns = 0;
	else if (HasHVerticalNeightbours(x, y) && x == 0)	turns = 1;
	else if (HasHVerticalNeightbours(x, y))				turns = 3;

	// for the sides
	else if (up && down)
	{
		if (x == 0) turns = 1;
		else		turns = 3;
	}
	else if (left && right)
	{
		if (y == 0) turns = 2;
		else		turns = 0;
	}

	return turns * turnAngle;
}
