#ifndef _TEXTUREMANAGER_HEADER_INCLUDED_
#define _TEXTUREMANAGER_HEADER_INCLUDED_

#include "../Game.h"

class TextureManager {
public:
	static SDL_Texture* LoadTexture(const char* fileName);
	static void Draw(SDL_Texture* texture, const SDL_Rect& src, const SDL_Rect& dest);
};


#endif // !_TEXTUREMANAGER_HEADER_INCLUDED_

