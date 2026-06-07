#include "SpriteSheet.h"

minigin::SpriteSheet::SpriteSheet(std::shared_ptr<Texture2D> texture, const glm::vec2& spriteSize)
	: m_SpriteSheet{ texture }
	, m_SpriteSize{ spriteSize }
	, m_SheetSize{ GetSpriteSheetDimensions(*texture.get(), spriteSize)}
{
}

SDL_Texture* minigin::SpriteSheet::GetSDLTexture() const
{
	return m_SpriteSheet->GetSDLTexture();
}

SDL_FRect minigin::SpriteSheet::GetSpriteCutout(const glm::ivec2& pos) const
{
	SDL_FRect rect{};
	rect.x = pos.x * m_SpriteSize.x;
	rect.y = pos.y * m_SpriteSize.y;
	rect.w = m_SpriteSize.x;
	rect.h = m_SpriteSize.y;

	return rect;
}

glm::ivec2 minigin::SpriteSheet::GetSpriteSheetDimensions(const Texture2D& texture, const glm::vec2& spriteSize) const
{
	glm::vec2 textureSize{};
	SDL_GetTextureSize(texture.GetSDLTexture(), &textureSize.x, &textureSize.y);

	glm::ivec2 dimensions{ textureSize / spriteSize };

	return dimensions;
}
