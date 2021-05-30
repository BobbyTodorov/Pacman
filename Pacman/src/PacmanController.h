#ifndef _PACMAN_HEADER_INCLUDED_
#define _PACMAN_HEADER_INCLUDED_

#include "Game.h"
#include "Engine/KeyboardController.h"

class PacmanController
{
private:
	Transform* transform;
	TextureRenderer* sr;
	Collider* collider;
	KeyboardController* keyboardController;

	SDL_Texture* lookingUp;
	SDL_Texture* lookingDown;
	SDL_Texture* lookingLeft;
	SDL_Texture* lookingRight;

	int speed;

	int direction[2];

	void InteractWithTile();

public:
	PacmanController() = default;
	PacmanController(const int& speed, Engine& engine, Entity& entity);
	
	bool CanMove();
	void Update();
	void GetUserInput();

	void Move();

	float GetPosX() const { return transform->xPos; }
	float GetPosY() const { return transform->yPos; }
};

#endif // !_PACMAN_HEADER_INCLUDED_

