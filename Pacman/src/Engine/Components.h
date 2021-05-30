#ifndef _COMPONENTS_HEADER_INCLUDED_
#define _COMPONENTS_HEADER_INCLUDED_

#include "SDL.h"

struct Transform
{
	int xPos;
	int yPos;
	float width;
	float height;
	//int scale;
};

struct TextureRenderer // basic SpriteRenderer
{
	SDL_Texture* defaultTexture;
};

struct Collider
{
	// from left upper corner (i.e. Entity's (0,0))
	SDL_Rect colliderRect;
	unsigned int collidedWith = NULL;
	bool isActive = true;
};


#endif // !_COMPONENTS_HEADER_INCLUDED_

