#ifndef _RENDERSYSTEM_HEADER_INCLUDED_
#define _RENDERSYSTEM_HEADER_INCLUDED_

#include "ECS.h"
#include "TextureManager.h"

// Draws the textures (currently positioned according to the Transform Component of the entity).
class RenderSystem : public System
{
public:
	void Render()
	{
		for (auto const& entity : entities)
		{
			if (!engine.HasComponent<TextureRenderer>(entity))
			{
				continue;
			}

			TextureRenderer renderer = engine.GetComponent<TextureRenderer>(entity);
			Transform transform = engine.GetComponent<Transform>(entity);

			SDL_Rect src, dest;

			src.x = src.y = 0;
			dest.x = transform.xPos;
			dest.y = transform.yPos;

			src.w = dest.w = transform.width;
			src.h = dest.h = transform.height;

			TextureManager::Draw(renderer.defaultTexture, src, dest);
		}
	}
};

#endif // !_RENDERSYSTEM_HEADER_INCLUDED_
