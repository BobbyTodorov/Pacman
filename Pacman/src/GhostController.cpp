#include "GhostController.h"
#include "Math.h"
#include "Engine/TextureManager.h"

#include <stdexcept>

const static int DIRECTIONS[4][2] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} }; //up, left, down, right

GhostController::GhostController(const int& speed, Engine& engine, Entity& attachedTo)
{
	this->speed = speed;
	this->mode = 0;

	this->direction[0] = 1;
	this->direction[1] = 0;
	this->previousDir = 0;
	this->prevTilePos[0] = 1;
	this->prevTilePos[1] = 1;
	
	if (!(engine.HasComponent<Transform>(attachedTo) &&
		engine.HasComponent<Collider>(attachedTo) &&
		engine.HasComponent<TextureRenderer>(attachedTo)))
	{
		std::cerr << "A ghost (currently) must have Transform, Collider and TextureRenderer components attached." << std::endl;
		exit(0);
	}

	transform = &engine.GetComponent<Transform>(attachedTo);
	collider = &engine.GetComponent<Collider>(attachedTo);
	sr = &engine.GetComponent<TextureRenderer>(attachedTo);

	normalTex = sr->defaultTexture;
	frightenedTex = TextureManager::LoadTexture("assets/Ghosts/frightened.png");
	eatenTex = TextureManager::LoadTexture("assets/Ghosts/eaten.png");
}

void GhostController::UpdateDirection()
{
	CoordinatesToTiles(xTileInd, yTileInd, transform->xPos, transform->yPos);
	
	// Do not enter house if Pacman is not inside. Otherwise a ghost might stuck in there if Pacman is located beneath the house.
	if (!Game::IsPacmanInGhostHouse() && xTileInd == Game::GHOST_HOUSE_DOOR_TILE_X && yTileInd == Game::GHOST_HOUSE_DOOR_TILE_Y)
	{
		shouldGoDown = false;
	}

	if (shouldGoUp) 
	{
		float dist = distFromEdge(transform->xPos, Game::TILE_SIZE);
		if (dist <= treshHold && dist >= -treshHold && Game::map->tiles[yTileInd - 1][xTileInd] != 1) // dist <= |treshHold| makes fixing the position look smoother
		{
			transform->xPos = xTileInd * Game::TILE_SIZE; // fixing the position, just to be precise
			direction[0] = 0;
			direction[1] = -1;
		}
	}

	else if (shouldGoLeft) 
	{
		float dist = distFromEdge(transform->yPos, Game::TILE_SIZE);
		if (dist <= treshHold && dist >= -treshHold && Game::map->tiles[yTileInd][xTileInd - 1] != 1)
		{
			transform->yPos = yTileInd * Game::TILE_SIZE;
			direction[0] = -1;
			direction[1] = 0;
		}
	}

	else if (shouldGoDown) 
	{
		float dist = distFromEdge(transform->xPos, Game::TILE_SIZE);
		if (dist <= treshHold && dist >= -treshHold && Game::map->tiles[yTileInd + 1][xTileInd] != 1)
		{
			transform->xPos = xTileInd * Game::TILE_SIZE;
			direction[0] = 0;
			direction[1] = 1;
		}
	}

	else if (shouldGoRight) 
	{
		float dist = distFromEdge(transform->yPos, Game::TILE_SIZE);
		if (dist <= treshHold && dist >= -treshHold && Game::map->tiles[yTileInd][xTileInd + 1] != 1)
		{
			transform->yPos = yTileInd * Game::TILE_SIZE;
			direction[0] = 1;
			direction[1] = 0;
		}
	}
}

int* getOpositeDirection(int direction[2])
{
	int* result = new int[2];

	if (direction[0] == 0 && direction[1] == -1) // Up -> Down
	{
		result[0] = 0;
		result[1] = 1;
	}
	else if (direction[0] == 0 && direction[1] == 1) // Down -> Up
	{
		result[0] = 0;
		result[1] = -1;
	}
	else if (direction[0] == -1 && direction[1] == 0) // Left -> Right
	{
		result[0] = 1;
		result[1] = 0;
	}
	else // Right -> Left
	{
		result[0] = -1;
		result[1] = 0;
	}

	return result;
}

// Algorithm: check adjacent tiles(except the previous one, the diagonal ones and ones containing a block) 
// and pick the one that would provide the shortest(or longest) distance towards the target... and give ghost that direction.
void GhostController::PickBestDirectionTowardsOrAwayFromTarget(const float& targetX, const float& targetY, const bool& isTowards)
{
	int posOfTileX; // position of the current tile in coordinates (pixels)
	int posOfTileY;
	
	CoordinatesToTiles(posOfTileX, posOfTileY, transform->xPos, transform->yPos);
	posOfTileX *= Game::TILE_SIZE;
	posOfTileY *= Game::TILE_SIZE;

	if (prevTilePos[0] == posOfTileX && prevTilePos[1] == posOfTileY)
	{
		return;
	}

	int* backwardsDir = getOpositeDirection(direction);

	int bestDirection[2] = { 0,0 };
	int bestDirectionDistance = -1;

	for (int i = 0; i < 4; ++i) // for each direction
	{
		int newDir[2] = { DIRECTIONS[i][0], DIRECTIONS[i][1] };
		if (newDir[0] == backwardsDir[0] && newDir[1] == backwardsDir[1]) // a ghost cannot start moving backwards 
		{
			continue;
		}

		float distance = 0;
		int posOfNewTileX = 0;
		int posOfNewTileY = 0;

		if ((newDir[0] == 1 && newDir[1] == 0) || (newDir[0] == 0 && newDir[1] == 1)) // Down/right
		{
			posOfNewTileX = posOfTileX + newDir[0] * Game::TILE_SIZE;
			posOfNewTileY = posOfTileY + newDir[1] * Game::TILE_SIZE;
		}
		else // Up/left
		{
			posOfNewTileX = posOfTileX + newDir[0];
			posOfNewTileY = posOfTileY + newDir[1];
		}

		distance = distBetweenTwoPoints(posOfNewTileX, posOfNewTileY, targetX, targetY);

		int indOfNewTileX = 0;
		int indOfNewTileY = 0;
		CoordinatesToTiles(indOfNewTileX, indOfNewTileY, posOfNewTileX, posOfNewTileY);

		if (Game::map->tiles[indOfNewTileY % tilesCountY][indOfNewTileX % tilesCountX] == 1) // a block there? -> don't calculate distance
		{
			continue;
		}

		if (bestDirection[0] == 0 && bestDirection[1] == 0) // first calculation (no previous values)
		{
			bestDirection[0] = newDir[0];
			bestDirection[1] = newDir[1];
			bestDirectionDistance = distance;
			continue;
		}

		if (isTowards)
		{
			if (distance < bestDirectionDistance)
			{
				bestDirection[0] = newDir[0];
				bestDirection[1] = newDir[1];
				bestDirectionDistance = distance;
			}
		}
		else
		{
			if (distance > bestDirectionDistance)
			{
				bestDirection[0] = newDir[0];
				bestDirection[1] = newDir[1];
				bestDirectionDistance = distance;
			}
		}
	}

	delete backwardsDir;

	if (bestDirection[0] == 1 && bestDirection[1] == 0) // right
	{
		shouldGoRight = true;
		shouldGoDown = shouldGoLeft = shouldGoUp = false;
	}
	else if (bestDirection[0] == -1 && bestDirection[1] == 0) // left
	{
		shouldGoLeft = true;
		shouldGoDown = shouldGoRight = shouldGoUp = false;
	}
	else if (bestDirection[0] == 0 && bestDirection[1] == 1) // down
	{
		shouldGoDown = true;
		shouldGoLeft = shouldGoRight = shouldGoUp = false;
	}
	else // up
	{
		shouldGoUp = true;
		shouldGoDown = shouldGoLeft = shouldGoRight = false;
	}

	prevTilePos[0] = posOfTileX;
	prevTilePos[1] = posOfTileY;
}

void GhostController::GetOutOfGhostHouse()
{
	PickBestDirectionTowardsOrAwayFromTarget(Game::GHOST_HOUSE_DOOR_XPOS, Game::GHOST_HOUSE_DOOR_YPOS, true);
}

void GhostController::Scatter()
{
	PickBestDirectionTowardsOrAwayFromTarget(rand() % Game::WINDOW_WIDTH, rand() % Game::WINDOW_HEIGHT, true);
}

void GhostController::SetMode(const int& mode) 
{
	collider->isActive = true;

	if (mode == 2)
	{
		sr->defaultTexture = frightenedTex;
	}
	else if (mode == 3)
	{
		sr->defaultTexture = eatenTex;
		collider->isActive = false;
	}
	else
	{
		sr->defaultTexture = normalTex;
	}

	if (mode == 1 || mode == 2)
	{
		timeVar = 0;
	}

	this->mode = mode;
}

void GhostController::UpdateBehavior(const int& targetXpos, const int& targetYpos)
{
	if (mode == 0) // scatter
	{
		if (timeVar < 0.3)
		{
			GetOutOfGhostHouse();
			timeVar += 0.01;
		}
		else
		{
			Scatter();
		}
	}
	else if (mode == 1) // chase
	{
		if (timeVar < 4)
		{
			PickBestDirectionTowardsOrAwayFromTarget(targetXpos, targetYpos, true);
		}
		else if (timeVar < 6)
		{
			Scatter();
		}
		else
		{
			timeVar = 0;
		}

		timeVar += 0.01;
	}
	else if (mode == 2) // frightened
	{
		if (timeVar < 6)
		{
			if (timeVar >= 4 && timeVar <= 5)
			{
				Scatter();
			}
			else
			{
				PickBestDirectionTowardsOrAwayFromTarget(targetXpos, targetYpos, false);
			}

			timeVar += 0.01;
		}
		else
		{
			SetMode(1);
		}
	}
	else // eaten
	{
		if (transform->xPos != Game::GHOST_HOUSE_DOOR_XPOS || transform->yPos != Game::GHOST_HOUSE_DOOR_YPOS)
		{
			PickBestDirectionTowardsOrAwayFromTarget(Game::GHOST_HOUSE_DOOR_XPOS, Game::GHOST_HOUSE_DOOR_YPOS, true);
		}
		else
		{
			SetMode(1);
		}
	}
}

void GhostController::Update(const int& targetXpos, const int& targetYpos)
{
	UpdateBehavior(targetXpos, targetYpos);

	UpdateDirection();

	transform->xPos += direction[0] * speed;
	transform->yPos += direction[1] * speed;
}