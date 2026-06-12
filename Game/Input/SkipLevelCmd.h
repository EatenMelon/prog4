#pragma once
#include <Command.h>

namespace digdug
{
	class SkipLevelCmd final : public minigin::Command
	{
	public:
		void Execute(const minigin::InputContext&, float) override;
	};
}