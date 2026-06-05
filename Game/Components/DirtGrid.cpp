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
		pos.y = cell / m_Width;

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
		auto& cell = m_Cells[col];

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
	auto origin = glm::vec2(GetOwner().GetWorldPosition());

	for (int c{ 0 }; c < static_cast<int>(m_Cells.size()); ++c)
	{
		glm::vec2 pos{};
		pos.x = static_cast<float>(c % m_Width);
		pos.y = static_cast<float>(c / m_Width);

		pos *= glm::vec2(m_CellSize);
		pos += origin;

		m_Cells[c].Render(pos, m_CellSize);
	}

	auto renderer = minigin::Renderer::GetInstance().GetSDLRenderer();

	SDL_FRect border{};
	border.x = origin.x;
	border.y = origin.y;
	border.w = border.x + m_Width * m_CellSize;
	border.h = border.y + m_Height * m_CellSize;
	SDL_RenderRect(renderer, &border);
}

void digdug::DirtGrid::GuiRender()
{
	bool open{ true };
	ImGui::Begin("sand box", &open);
	{
		glm::ivec2 startA{ 1, 1 };
		glm::ivec2 endA{ 4, 4 };

		if (ImGui::Button("Dig Tunnel A => x")) Dig(startA, endA, 'x');
		if (ImGui::Button("Dig Tunnel A => y")) Dig(startA, endA, 'y');

		glm::ivec2 startB{ 6, 6 };
		glm::ivec2 endB{ 10, 10 };

		if (ImGui::Button("Dig Tunnel B => x")) Dig(startB, endB, 'x');
		if (ImGui::Button("Dig Tunnel B => y")) Dig(startB, endB, 'y');

		glm::ivec2 startC{ 2, 5 };
		glm::ivec2 endC{ 11, 14 };

		if (ImGui::Button("Dig Tunnel C => x")) Dig(startC, endC, 'x');
		if (ImGui::Button("Dig Tunnel C => y")) Dig(startC, endC, 'y');
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

	//glm::ivec2 dir{ startPos - endPos };
	const int increment{ signbit(diff)? 1 : -1 };
	//std::cout << "dir " << axis << ": " << diff << ", increment = " << increment << "\n";

	for (int axisValue{ start }; axisValue <= end; axisValue += increment)
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
			index = axisValue + oppositeAxis * m_Width;
			entryPoint = glm::ivec2{ dir, 0 };
			break;

		case 'Y':
		case 'y':
			index = oppositeAxis + axisValue * m_Width;
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
