#pragma once

#include <Command.h>
#include <DirtGrid.h>
#include <glm/glm.hpp>

namespace digdug
{
	class GridMoveCmd final : public minigin::ActorCommand
	{
	public:
		GridMoveCmd(minigin::GameObject* pActor, int playerID, DirtGrid& grid, float speed, bool canDig = true);
		void SetGridPosition(const glm::ivec2& gridPos);

	protected:
		void ActorExecute(const minigin::InputContext& context, float deltaTime) override;

	private:
		glm::vec3 KeepInBounds(const glm::vec3& pos) const;
		glm::ivec2 ChooseDirection(const glm::vec2& axis);

		DirtGrid* m_Grid{ nullptr };

		float m_MovementSpeed{ 1.f };
		bool m_CanDig{ false };

		glm::ivec2 m_PosInGrid{ 0, 0 };
		glm::ivec2 m_MoveDirection{ 0, 0 };
		glm::vec2 m_ActorSize{ 0.f, 0.f };

		int m_LastFrame{ 0 };
	};
}