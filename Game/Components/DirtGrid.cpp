#include "DirtGrid.h"
#include <GameObject.h>
#include <imgui.h>

#include <stdexcept>
#include <Renderer.h>
#include <cmath>

void digdug::DirtGrid::Start()
{
	m_Cells.reserve(m_Width * m_Height);

	for (int cell{ 0 }; cell < m_Width * m_Height; ++cell)
	{
		glm::ivec2 pos{};
		pos.y = m_Height - 1 - (cell / m_Width);

		auto depth = GetDepthLevel(pos);

		if (!m_TileTextures.contains(depth))
		{
			throw std::runtime_error("Depth doesn't have a Matching tile texture!");
		}

		m_Cells.emplace_back(*m_TileTextures[depth]);
	}

	// dig out the top of the grid
	for (int col{ 0 }; col < m_Width; ++col)
	{
		auto& cell = m_Cells[col + (m_Height - 1) * m_Width];

		cell.Dig(glm::vec2{ 1, -1 });
		cell.Dig(glm::vec2{ -1, 0 });
	}
}

void digdug::DirtGrid::Update(float)
{
	if (m_DiggingQueue.empty()) return;

	auto& digEvent = m_DiggingQueue.front();
	DigTunnel(digEvent);
	m_DiggingQueue.pop();
}

void digdug::DirtGrid::Render() const
{
	auto renderer = minigin::Renderer::GetInstance().GetSDLRenderer();

	auto origin = glm::vec2(GetOwner().GetLocalPosition());

	for (int c{ 0 }; c < static_cast<int>(m_Cells.size()); ++c)
	{
		glm::vec2 pos{};
		pos.x = static_cast<float>(c % m_Width);
		pos.y = static_cast<float>(m_Height - 1 - (c / m_Width));

		pos *= glm::vec2(m_CellSize);
		pos += origin;

		m_Cells[c].Render(pos, m_CellSize);

		if (!m_ShowOutlines) continue;

		SDL_SetRenderDrawColor(renderer, 255, 125, 255, 255);

		SDL_FRect rect{};
		rect.x = pos.x;
		rect.y = pos.y;
		rect.w = m_CellSize;
		rect.h = m_CellSize;

		SDL_RenderRect(renderer, &rect);
	}
}

void digdug::DirtGrid::GuiRender()
{
	bool open{ true };
	ImGui::Begin("dirt grid", &open);
	{
		bool enable{ m_ShowOutlines };
		if (ImGui::RadioButton("Outlines", &enable))
		{
			m_ShowOutlines = !m_ShowOutlines;
		}
	}
	ImGui::End();
}

void digdug::DirtGrid::Dig(const glm::ivec2& start, const glm::ivec2& end, const char preferredAxis)
{
	DigEvent newEvent{};

	newEvent.start = start;
	newEvent.end = end;
	newEvent.preferredAxis = preferredAxis;

	m_DiggingQueue.push(newEvent);
}

bool digdug::DirtGrid::HasBeenDug(const glm::ivec2& gridPos) const
{
	int idx{ gridPos.x + (m_Height - gridPos.y - 1) * m_Width };

	if (idx < 0) return false;
	if (idx >= static_cast<int>(m_Cells.size())) return false;

	return m_Cells[idx].HasBeenDug();
}

glm::vec3 digdug::DirtGrid::GetCellLocalPos(const glm::ivec2& gridPos) const
{
	glm::vec3 pos{};

	pos.x = gridPos.x * m_CellSize;
	pos.y = gridPos.y * m_CellSize;

	return pos;
}

glm::ivec2 digdug::DirtGrid::GetPosInGrid(const glm::vec3& pos) const
{
	glm::vec3 local = pos;

	glm::vec2 grid = local / m_CellSize;
	grid = glm::floor(grid);

	return glm::ivec2(grid);
}

void digdug::DirtGrid::SetTileTexture(Depth depth, minigin::Texture2D& texture)
{
	m_TileTextures.insert_or_assign(depth, &texture);
}

digdug::DirtGrid::Depth digdug::DirtGrid::GetDepthLevel(const glm::ivec2& pos) const
{
	constexpr int levels{ 4 };

	constexpr int surfaceY{ 0 };
	const int outerHeights{ m_Height / levels };
	const int middleHeights{ outerHeights + 1 };

	if (pos.y == surfaceY)
	{
		return Depth::Surface;
	}
	else if (pos.y <= outerHeights)
	{
		return Depth::TopSoil;
	}
	else if (pos.y <= outerHeights + middleHeights)
	{
		return Depth::SubSoil;
	}
	else if (pos.y <= outerHeights + middleHeights * 2)
	{
		return Depth::Stone;
	}

	return Depth::Bedrock;
}

void digdug::DirtGrid::DigTunnel(const DigEvent& digEvent)
{
	switch (digEvent.preferredAxis)
	{
	case 'X':
	case 'x':
		DigOneWay(digEvent.start.x, digEvent.end.x, digEvent.start.y, 'x');
		DigOneWay(digEvent.start.y, digEvent.end.y, digEvent.end.x, 'y');
		break;

	case 'Y':
	case 'y':
		DigOneWay(digEvent.start.y, digEvent.end.y, digEvent.start.x, 'y');
		DigOneWay(digEvent.start.x, digEvent.end.x, digEvent.end.y, 'x');
		break;

	default:
		std::cout << "No such axis found, resuming with X-axis!";
		Dig(digEvent.start, digEvent.end, digEvent.preferredAxis);
		break;
	}
}

void digdug::DirtGrid::DigOneWay(int start, int end, int oppositeAxis, const char axis)
{
	int diff = start - end;
	if (diff == 0) return;

	const int increment{ signbit(diff)? 1 : -1 };

	auto condition = [&](int value) -> bool
		{
			if (start < end)
			{
				return value <= end;
			}
			return value >= end;
		};

	for (int axisValue{ start }; condition(axisValue); axisValue += increment)
	{
		int index{ -1 };
		int dir{ signbit(diff) ? -1 : 1 };

		if (axisValue == start)
		{
			dir = -dir;
		}

		glm::ivec2 entryPoint{};

		switch (axis)
		{
		case 'X':
		case 'x':
			index = axisValue + (m_Height - oppositeAxis - 1) * m_Width;
			entryPoint = glm::ivec2{ dir, 0 };
			break;

		case 'Y':
		case 'y':
			index = oppositeAxis + (m_Height - axisValue - 1) * m_Width;
			entryPoint = glm::ivec2{ 0, dir };
			break;
		}

		if (index < 0) continue;
		if (index >= static_cast<int>(m_Cells.size())) continue;

		m_Cells[index].Dig(entryPoint);

		if (axisValue != end && axisValue != start)
		{
			m_Cells[index].Dig(-entryPoint);
		}
	}
}
