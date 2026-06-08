#pragma once
#include <Command.h>
#include <functional>

namespace digdug
{
	class Harpoon;
	class HarpoonCmd final : public minigin::ActorCommand
	{
	public:
		HarpoonCmd(int playerID, Harpoon* harpoon, const std::function<void(Harpoon*)>& callback);

	private:
		void ActorExecute(const minigin::InputContext&, float) override;

		Harpoon* m_Harpoon{ nullptr };
		std::function<void(Harpoon*)> m_Callback{};
	};
}
