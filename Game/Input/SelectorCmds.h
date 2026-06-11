#pragma once
#include <Command.h>

namespace digdug
{
	class ObjectSelector;
	class SelectCmd final : public minigin::Command
	{
	public:
		SelectCmd(ObjectSelector* selector, float delay = 0.f);

		void Execute(const minigin::InputContext& context, float deltaTime) override;

	private:
		ObjectSelector* m_Selector{ nullptr };
		int m_LastFrame{ 0 };
		const float m_Delay{ 0.f };
		float m_TimeUntilSelect{ 0.f };
	};

	class SubmitCmd final : public minigin::Command
	{
	public:
		SubmitCmd(ObjectSelector* selector);

		void Execute(const minigin::InputContext&, float) override;

	private:
		ObjectSelector* m_Selector{ nullptr };
	};
}