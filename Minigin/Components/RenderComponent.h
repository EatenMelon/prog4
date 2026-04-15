#pragma once
#include <string>

#include "Component.h"
#include <memory>

namespace dae
{
	class Texture2D;
	class RenderComponent final: public Component
	{
	public:
		// reuse the constructor of the base component class
		using Component::Component;

		void Render() const override;

		void SetTexture(const std::string& filename);
		void SetTexture(std::shared_ptr<Texture2D> texture);

	private:

		std::shared_ptr<Texture2D> m_Texture{};
	};
}