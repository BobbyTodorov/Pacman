#include "Game.h"

Game* game = nullptr;

int main(int argc, char* argv[])
{

	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	Uint32 frameStart;
	int frameTime;

	game = new Game();
	game->Init();

	while (game->Running())
	{

		frameStart = SDL_GetTicks();

		game->HandleEvents();
		game->Update();
		game->Render(); // Generally, rendering could be happening more often than ~60 times per second. But in this case it is fine.

		frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
	}
	game->Clean();
	return 0;
}