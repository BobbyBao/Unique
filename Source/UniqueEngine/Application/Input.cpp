#include "UniquePCH.h"
#include "Input.h"
#include <SDL/SDL.h>

namespace Unique
{
	Input::Input()
	{
	}

	Input::~Input()
	{
	}

	bool Input::ProcessEvents()
	{
		SDL_Event event;
		/* Check for events */
		/*SDL_WaitEvent(&event); emscripten does not like waiting*/

		while (SDL_PollEvent(&event))
		{
			switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				break;
			case SDL_TEXTEDITING:
				break;
			case SDL_TEXTINPUT:
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			case SDL_QUIT:
				return false;
				break;
			default:
				break;
			}
		}

		return true;
	}

}
