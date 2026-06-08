#pragma once
#include <Command.h>
#include <functional>

namespace digdug
{
	class Harpoon;
	class HarpoonCmd final : public minigin::Command
	{
	public:
		HarpoonCmd(Harpoon* harpoon, const std::function<void(Harpoon*)>& callback);
		void Execute(const minigin::InputContext&, float) override;

	private:
		Harpoon* m_Harpoon{ nullptr };
		std::function<void(Harpoon*)> m_Callback{};
	};
}
