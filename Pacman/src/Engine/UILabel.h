#ifndef _UILABELCOMPONENT_HEADER_INCLUDED_
#define _UILABELCOMPONENT_HEADER_INCLUDED_

#include "SDL_ttf.h"

struct UILabel
{
	static SDL_Texture* CreateTextLabelTexture(std::string text, TTF_Font* font, const SDL_Color& color)
	{
		SDL_Surface* tempSurface = TTF_RenderText_Blended(font, text.c_str(), color);
		if (!tempSurface)
		{
			std::cerr << SDL_GetError() << " TTF_RenderText_Blended" << std::endl;
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
		if (!texture)
		{
			std::cerr << SDL_GetError() << " SDL_CreateTexture in SetLabelText" << std::endl;
		}

		SDL_FreeSurface(tempSurface);

		return texture;
		//SDL_QueryTexture(texture, nullptr, nullptr, &position.w, &position.h);
	}
};

#endif // !_UILABELCOMPONENT_HEADER_INCLUDED_

