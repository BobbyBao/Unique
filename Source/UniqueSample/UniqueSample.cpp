// UniqueSample.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <SDL/SDL.h>

int done = 0;
void
loop()
{
	SDL_Event event;
	/* Check for events */
	/*SDL_WaitEvent(&event); emscripten does not like waiting*/

	while (SDL_PollEvent(&event)) {
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
			done = 1;
			break;
		default:
			break;
		}
	}
#ifdef __EMSCRIPTEN__
	if (done) {
		emscripten_cancel_main_loop();
	}
#endif
}

int
main(int argc, char *argv[])
{
	SDL_Window *window;

	/* Enable standard application logging */
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return (1);
	}

	/* Set 640x480 video mode */
	window = SDL_CreateWindow("Unique Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480, 0);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create 640x480 window: %s\n",
			SDL_GetError());

		SDL_Quit();
		return 2;
	}

#if __IPHONEOS__
	/* Creating the context creates the view, which we need to show keyboard */
	SDL_GL_CreateContext(window);
#endif


#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(loop, 0, 1);
#else
	while (!done) {
		loop();
	}
#endif

	SDL_Quit();
    return 0;
}

