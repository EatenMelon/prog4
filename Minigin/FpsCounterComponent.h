#pragma once
#include "TextComponent.h"

namespace dae
{
	class FpsCounterComponent final : public TextComponent
	{
	public:
		using TextComponent::TextComponent;

		void Update(float deltaTime) override;

	private:

		float m_UntilTextUpdate{ 0.f };
	};
}