#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <Texture2D.h>

namespace minigin
{
	class SpriteSheet final
	{
	public:
		SpriteSheet(std::shared_ptr<Texture2D> texture, const glm::vec2& spriteSize);

		SDL_Texture* GetSDLTexture() const;
		SDL_FRect GetSpriteCutout(const glm::ivec2& pos) const;

		const glm::vec2& GetSpriteSize() const { return m_SpriteSize; }
		const glm::ivec2& GetSheetSize() const { return m_SheetSize; }

		SpriteSheet(const SpriteSheet&) = delete;
		SpriteSheet& operator=(const SpriteSheet&) = delete;
		SpriteSheet(SpriteSheet&&) = delete;
		SpriteSheet& operator=(SpriteSheet&&) = delete;

		~SpriteSheet() = default;

	private:
		glm::ivec2 GetSpriteSheetDimensions(const Texture2D& texture, const glm::vec2& spriteSize) const;

		std::shared_ptr<Texture2D> m_SpriteSheet{ nullptr };
		const glm::vec2 m_SpriteSize{};
		const glm::ivec2 m_SheetSize{};
	};
}