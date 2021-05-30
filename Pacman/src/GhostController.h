#ifndef _GHOSTBEHAVIOR_HEADER_INCLUDED_
#define _GHOSTBEHAVIOR_HEADER_INCLUDED_

#include "Game.h"

class GhostController
{
private:
	Transform* transform;
	Collider* collider;
	TextureRenderer* sr;

	SDL_Texture* normalTex;
	SDL_Texture* frightenedTex;
	SDL_Texture* eatenTex;

	int speed;

	int direction[2];
	int previousDir;

	int prevTilePos[2];

	int mode; //0 - shatter, 1 - chase , 2 - frightened, 3 - eaten

	int xTileInd = 0; // used to store a tile index when needed
	int yTileInd = 0; //

	float treshHold = 0.1; // the minimal required distance to a block edge required for making a turn around it possible

	bool shouldGoUp = false;
	bool shouldGoLeft = false;
	bool shouldGoRight = false;
	bool shouldGoDown = false;

	float timeVar = 0; // used for simulating a timer

	void UpdateDirection();
	void GetOutOfGhostHouse();
	void Scatter();
	void PickBestDirectionTowardsOrAwayFromTarget(const float& targetX, const float& targetY, const bool& isTowards);
	void UpdateBehavior(const int& targetXpos, const int& targetYpos);

public:
	GhostController() = default;
	GhostController(const int& speed, Engine& engine, Entity& attachedTo);

	void SetMode(const int& mode);
	int GetMode() const { return mode; }
	void SetSpeed(const int& speed) { this->speed = speed; }

	void Update(const int& targetXpos, const int& targetYpos);
};


#endif // !_GHOSTBEHAVIOR_HEADER_INCLUDED_

