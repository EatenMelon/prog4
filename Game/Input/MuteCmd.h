#pragma once
#include <Command.h>

namespace digdug
{
	class MuteCmd final : public minigin::Command
	{
	public:
		void Execute(const minigin::InputContext&, float) override;
	};
}