#pragma once
#include <Command.h>
#include <ObjectSelector.h>

namespace minigin
{
	class TextComponent;
	class GameObject;
}

namespace digdug
{
	class LetterSelectCmd final : public minigin::Command
	{
	public:
		LetterSelectCmd(ObjectSelector* selector, float delay = 0.f);

		void Execute(const minigin::InputContext& context, float deltaTime) override;

	private:
		ObjectSelector* m_Selector{ nullptr };

		const char m_StartChar{ 'A' };
		const char m_EndChar{ 'Z' };

		int m_LastFrame{ 0 };
		const float m_Delay{ 0.f };
		float m_TimeUntilSelect{ 0.f };
	};
}