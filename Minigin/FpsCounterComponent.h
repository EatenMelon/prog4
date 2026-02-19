#pragma once
#include "Component.h"

namespace dae
{
	class TextComponent;
	class FpsCounterComponent final : public Component
	{
	public:
		using Component::Component;

		void Init() override;
		void Update(float deltaTime) override;

	private:

		float m_UntilTextUpdate{ 10.f };

		TextComponent* m_TextComponent{ nullptr };
	};
}