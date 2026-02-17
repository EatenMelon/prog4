#pragma once
#include <string>

#include "Component.h"

namespace dae
{
	class Texture2D;
	class RenderComponent final: public Component
	{
	public:
		using Component::Component;

		void Render() const override;

		void SetTexture(const std::string& filename);

	private:

		std::shared_ptr<Texture2D> m_texture{};
	};
}