#include "PacmanController.h"
#include "Math.h"
#include "Engine/TextureManager.h";

// its OK for these to be shared amoung functions and be global as 
// there is only one Pacman and the execution is linear
const static float treshHold = 0.1; // the value that distance (to the edge of block) has to be before making a turn around that edge's corner

int yTileInd = 0; 
int xTileInd = 0; 

PacmanController::PacmanController(const int& speed, Engine& engine, Entity& attachedTo) : speed(speed)
{
	this->direction[0] = 1;
	this->direction[1] = 0;
	
	if (!(engine.HasComponent<Transform>(attachedTo) &&
		engine.HasComponent<Collider>(attachedTo) &&
		engine.HasComponent<TextureRenderer>(attachedTo) &&
		engine.HasComponent<KeyboardController>(attachedTo)))
	{
		std::cerr << "Pacman (currently) must have Transform, Collider, TextureRenderer and KeyboardController components attached." << std::endl;
		exit(0);
	}

	transform = &engine.GetComponent<Transform>(attachedTo);
	sr = &engine.GetComponent<TextureRenderer>(attachedTo);
	collider = &engine.GetComponent<Collider>(attachedTo);
	keyboardController = &engine.GetComponent <KeyboardController>(attachedTo);

	lookingUp = TextureManager::LoadTexture("assets/Pacman/lookingUp.png");
	lookingDown = TextureManager::LoadTexture("assets/Pacman/lookingDown.png");
	lookingLeft = TextureManager::LoadTexture("assets/Pacman/lookingLeft.png");
	lookingRight = TextureManager::LoadTexture("assets/Pacman/lookingRight.png");
}

void PacmanController::InteractWithTile()
{
	CoordinatesToTiles(xTileInd, yTileInd, transform->xPos, transform->yPos);

	if (direction[0] == 0 && direction[1] == 1 || direction[0] == 0 && direction[1] == -1) // Down/Up
	{
		CoordinatesToTiles(xTileInd, yTileInd, transform->xPos, transform->yPos + Game::HALF_TILE_SIZE);
	}
	else // Left/Right
	{
		CoordinatesToTiles(xTileInd, yTileInd, transform->xPos + Game::HALF_TILE_SIZE, transform->yPos);
	}

	int* tileValue = &Game::map->tiles[yTileInd % tilesCountY][xTileInd % tilesCountX];

	if (*tileValue == 2)
	{
		*tileValue = 0;

		Game::IncrementScore();
	}
	else if (*tileValue == 3)
	{
		*tileValue = 0;
		
		Game::FrightenGhosts();
	}
}

void PacmanController::GetUserInput()
{
	CoordinatesToTiles(xTileInd, yTileInd, transform->xPos, transform->yPos);

	if (keyboardController->wasUpPressed) {
		float dist = distFromEdge(transform->xPos, Game::TILE_SIZE);
		if (dist <= treshHold && dist >= - treshHold && Game::map->tiles[yTileInd - 1][xTileInd] != 1)
		{
			transform->xPos = xTileInd * Game::TILE_SIZE;
			sr->defaultTexture = lookingUp;
			direction[0] = 0;
			direction[1] = -1;
		}
	}

	else if (keyboardController->wasLeftPressed) {
		float dist = distFromEdge(transform->yPos, Game::TILE_SIZE);
		if (dist <= treshHold && dist >= - treshHold && Game::map->tiles[yTileInd][xTileInd - 1] != 1)
		{
			transform->yPos = yTileInd * Game::TILE_SIZE;
			sr->defaultTexture = lookingLeft;
			direction[0] = -1;
			direction[1] = 0;
		}
	}

	else if (keyboardController->wasDownPressed) {
		float dist = distFromEdge(transform->xPos, Game::TILE_SIZE);
		if (dist <= treshHold && dist >= - treshHold && Game::map->tiles[yTileInd + 1][xTileInd] != 1)
		{
			transform->xPos = xTileInd * Game::TILE_SIZE;
			sr->defaultTexture = lookingDown;
			direction[0] = 0;
			direction[1] = 1;
		}
	}

	else if (keyboardController->wasRightPressed) {
		float dist = distFromEdge(transform->yPos, Game::TILE_SIZE);
		if (dist <= treshHold && dist >= - treshHold && Game::map->tiles[yTileInd][xTileInd + 1] != 1)
		{
			transform->yPos = yTileInd * Game::TILE_SIZE;
			sr->defaultTexture = lookingRight;
			direction[0] = 1;
			direction[1] = 0;
		}
	}
}

bool PacmanController::CanMove()
{
	if ((direction[0] == 1 && direction[1] == 0) || (direction[0] == 0 && direction[1] == 1)) // Down/right 
	{
		CoordinatesToTiles(xTileInd, yTileInd, transform->xPos + direction[0] * Game::TILE_SIZE, transform->yPos + direction[1] * Game::TILE_SIZE);
	}
	else // Up/left 
	{
		CoordinatesToTiles(xTileInd, yTileInd, transform->xPos + direction[0], transform->yPos + direction[1]);
	}

	if (Game::map->tiles[yTileInd % tilesCountY][xTileInd % tilesCountX] == 1) {
		return false;
	}

	return true;
}

void PacmanController::Move()
{
	transform->xPos += direction[0] * speed;
	transform->yPos += direction[1] * speed;

	InteractWithTile();
}

void PacmanController::Update()
{
	if (collider->collidedWith) // Except Pacman, there are only ghosts with colliders. Heavy collision systems are not needed.
	{
		Game::PacmanCollidingWith(collider->collidedWith);
		collider->collidedWith = NULL;
	}	

	keyboardController->Update();
	GetUserInput();

	if (CanMove())
	{
		Move();
	}
}
