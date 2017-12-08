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
				//PrintKey(&event.key.keysym, (event.key.state == SDL_PRESSED) ? SDL_TRUE : SDL_FALSE, (event.key.repeat) ? SDL_TRUE : SDL_FALSE);
				break;
			case SDL_TEXTEDITING:
				//PrintText("EDIT", event.text.text);
				break;
			case SDL_TEXTINPUT:
				//PrintText("INPUT", event.text.text);
				break;
			case SDL_MOUSEBUTTONDOWN:
				/* Any button press quits the app... */
			case SDL_QUIT:

#ifdef __EMSCRIPTEN__
				emscripten_cancel_main_loop();
#endif
				return false;
				break;
			default:
				break;
			}
		}

		return true;
	}

}
