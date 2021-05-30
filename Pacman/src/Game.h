#ifndef _GAME_HEADER_INCLUDED_
#define _GAME_HEADER_INCLUDED_

#include "Engine/ECS.h"
#include "Map.h"

#include "SDL.h"


static Engine engine;

class Game
{
public:
	static const unsigned TILE_SIZE;
	static const unsigned HALF_TILE_SIZE;
	static const unsigned WINDOW_WIDTH;
	static const unsigned WINDOW_HEIGHT;
	static const unsigned GHOST_HOUSE_DOOR_TILE_X;
	static const unsigned GHOST_HOUSE_DOOR_TILE_Y;
	static const unsigned GHOST_HOUSE_DOOR_XPOS;
	static const unsigned GHOST_HOUSE_DOOR_YPOS;

	static SDL_Renderer* renderer;
	static SDL_Event event;
	static Map* map;

	static bool isRunning;

	Game();
	~Game();

	static void PacmanCollidingWith(const Entity& ghost);
	static bool IsPacmanInGhostHouse();
	static void FrightenGhosts();
	static void IncrementScore();

	void Init();

	void HandleEvents();
	void Update();
	void Render();
	bool Running() const { return isRunning; }
	void Clean();

private:
	static unsigned currentScore;
	static bool isWinOrLoseConditionReached;

	SDL_Window* window = nullptr;
	void UpdateDifficulty();
	void UpdateGhosts();
};

#endif // !_GAME_HEADER_INCLUDED_
