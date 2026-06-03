#pragma once
#include <string>
#include <glm/vec2.hpp>

#include "Component.h"
#include <memory>

namespace minigin
{
	class Texture2D;
	class RenderComponent final: public Component
	{
	public:
		using Component::Component;

		void Render() const override;

		void SetTexture(const std::string& filename);
		void SetTexture(std::shared_ptr<Texture2D> texture);

		void SetUniformScale(float scale);
		void SetRotation(float degrees) { m_Rotation = degrees; };
		void SetHorizontalFlip(bool flip) { m_HorFlip = flip; };
		void SetVerticalFlip(bool flip) { m_VerFlip = flip; };

		glm::vec2 GetSize() const;

	private:

		std::shared_ptr<Texture2D> m_Texture{};
		float m_UniformScale{ 1.f };
		float m_Rotation{ 0.f };
		bool m_HorFlip{ false };
		bool m_VerFlip{ false };

	};
}