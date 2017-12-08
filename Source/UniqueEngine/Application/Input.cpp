#include <SDL/SDL.h>
#include "Input.h"

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
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
					SDL_Log("Window %d resized to %dx%d",
						event.window.windowID, event.window.data1,
						event.window.data2);

					//GetSubsystem<Graphics>().Resize(IntVector2(event.window.data1, event.window.data2));
					break;

				default:
					//SDL_Log("Window %d event %d",
					//	event.window.windowID, event.window.event);
					break;
				}
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
