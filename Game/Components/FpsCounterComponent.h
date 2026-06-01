#pragma once
#include "Component.h"

namespace minigin
{
	class TextComponent;
}

namespace digdug
{
	class FpsCounterComponent final : public minigin::Component
	{
	public:
		using minigin::Component::Component;

		void Init() override;
		void Update(float deltaTime) override;

	private:

		float m_UntilTextUpdate{ 0.f };

		minigin::TextComponent* m_TextComponent{ nullptr };
	};
}