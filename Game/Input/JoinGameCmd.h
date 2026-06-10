#pragma once
#include <Command.h>

namespace digdug
{
	class JoinGameCmd final : public minigin::Command
	{
	public:
		void Execute(const minigin::InputContext& context, float) override;
	};
}