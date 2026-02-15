#pragma once
#include "Component.h"

namespace dae
{
	class FpsCounterComponent final : public Component
	{
	public:

		void Update(float deltaTime) override;

	private:


	};
}