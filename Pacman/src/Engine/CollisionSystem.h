#ifndef _COLLISIONSYSTEM_HEADER_INCLUDED_
#define _COLLISIONSYSTEM_HEADER_INCLUDED_

#include "ECS.h"

class CollisionSystem : public System
{
private:
	int offsetX = 6;
	int offsetY = 6;

	void UpdateColliders()
	{
		for (auto const& entity : entities)
		{
			if (!engine.HasComponent<Collider>(entity))
			{
				continue;
			}

			auto& collider = engine.GetComponent<Collider>(entity);

			auto& transform = engine.GetComponent<Transform>(entity);

			// offset is currently fixed! as there is (and will be) no reason for it not to be
			collider.colliderRect.x = transform.xPos + offsetX; 
			collider.colliderRect.y = transform.yPos + offsetY;
		}
	}

	bool AABB(const SDL_Rect& rectA, const SDL_Rect& rectB)
	{
		return
			rectA.x + rectA.w >= rectB.x &&
			rectB.x + rectB.w >= rectA.x &&
			rectA.y + rectA.h >= rectB.y &&
			rectB.y + rectB.h >= rectA.y;
	}

public:
	void CheckForCollisions()
	{
		UpdateColliders();

		for (auto const& entityA : entities)
		{
			if (!engine.HasComponent<Collider>(entityA))
			{
				continue;
			}

			auto colliderA = &engine.GetComponent<Collider>(entityA);

			if (colliderA->isActive == false)
			{
				continue;
			}

			for (auto const& entityB : entities)
			{
				if (entityA == entityB)
				{
					continue;
				}

				if (!engine.HasComponent<Collider>(entityB))
				{
					continue;
				}

				auto colliderB = &engine.GetComponent<Collider>(entityB);

				if (colliderB->isActive == false)
				{
					continue;
				}
				
				if (AABB(colliderA->colliderRect, colliderB->colliderRect))
				{
					colliderA->collidedWith = entityB;
					colliderB->collidedWith = entityA;
				}
			}
		}
	}
};

#endif // !_COLLISIONSYSTEM_HEADER_INCLUDED_
