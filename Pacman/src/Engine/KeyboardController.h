#ifndef _KEYBOARDCONTROLLER_HEADER_INCLUDED_
#define _KEYBOARDCONTROLLER_HEADER_INCLUDED_

#include "../Game.h"
#include "Components.h"

class KeyboardController
{
public:
	bool wasUpPressed;
	bool wasDownPressed;
	bool wasLeftPressed;
	bool wasRightPressed;

	void Update()
	{
		if (Game::event.type == SDL_KEYDOWN)
		{
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_UP:
				wasUpPressed = true;
				wasDownPressed = wasLeftPressed = wasRightPressed = false;
				break;
			case SDLK_DOWN:
				wasDownPressed = true;
				wasUpPressed = wasLeftPressed = wasRightPressed = false;
				break;
			case SDLK_LEFT:
				wasLeftPressed = true;
				wasRightPressed = wasUpPressed = wasDownPressed = false;
				break;
			case SDLK_RIGHT:
				wasRightPressed = true;
				wasLeftPressed = wasUpPressed = wasDownPressed = false;
				break;
			default:
				break;
			}
		}
	}
};

#endif // !_KEYBOARDCONTROLLER_HEADER_INCLUDED_
