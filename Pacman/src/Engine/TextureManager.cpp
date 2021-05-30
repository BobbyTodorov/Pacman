#include "TextureManager.h"

#include "SDL_image.h"

SDL_Texture* TextureManager::LoadTexture(const char* fileName)
{
	SDL_Surface* surface = IMG_Load(fileName);
	if (!surface)
	{
		std::cerr << SDL_GetError() << " Img_Load in LoadTexture" << std::endl;
		return nullptr;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::renderer, surface);
	if (!texture)
	{
		std::cerr << SDL_GetError() << " CreateTextureFromSurface in LoadTexture" << std::endl;
		return nullptr;
	}

	SDL_FreeSurface(surface);

	return texture;
}

void TextureManager::Draw(SDL_Texture* texture, const SDL_Rect& src, const SDL_Rect& dest)
{
	SDL_RenderCopy(Game::renderer, texture, &src, &dest);
}
