#pragma once
#include "Component.h"

namespace dae
{
	class CacheTestComponent final : public Component
	{
	public:
		using Component::Component;

		void GuiRender() const override;

	private:

	};
}