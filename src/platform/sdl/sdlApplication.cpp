#include "sdlApplication.hpp"
#include "core/common.hpp"
#include <SDL2/SDL.h>
#include <iostream>

uint32 SDLApplication::numInstances = 0;

SDLApplication* SDLApplication::create()
{
	const uint32 flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER;
	uint32 initialized = SDL_WasInit(flags);
	if(initialized != flags &&
			SDL_Init(flags) != 0) {
		DEBUG_LOG("SDLApplication", LOG_ERROR, "SDL_Init: %s", SDL_GetError());
		return nullptr;
	}

	return new SDLApplication();
}

SDLApplication::SDLApplication()
{
#ifdef SDL_JOYSTICK_DISABLED
	DEBUG_LOG("", "NONE", "well fuck");
#endif
	numInstances++;
	isAppRunning = true;
}

SDLApplication::~SDLApplication()
{
	numInstances--;
	if(numInstances == 0) {
		SDL_Quit();
	}
}

void SDLApplication::processMessages(double delta, IApplicationEventHandler& eventHandler)
{
	SDL_Event e;
	(void)delta;
	
	while(SDL_PollEvent(&e)) {
		switch(e.type){
			case SDL_KEYDOWN:
			eventHandler.onKeyDown(e.key.keysym.scancode, e.key.repeat != 0);
			break;
		case SDL_KEYUP:
			eventHandler.onKeyUp(e.key.keysym.scancode, e.key.repeat != 0);
			break;
		case SDL_MOUSEBUTTONDOWN:
			eventHandler.onMouseDown(e.button.button, e.button.clicks);
			break;
		case SDL_MOUSEBUTTONUP:
			eventHandler.onMouseUp(e.button.button, e.button.clicks);
			break;
		case SDL_MOUSEMOTION:
			eventHandler.onMouseMove(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
			break;
		case SDL_QUIT:
			isAppRunning = false;
			break;

		//controller "support" by Shinigami072
		case SDL_CONTROLLERDEVICEADDED: {
			int resp = eventHandler.onControllerAdded();
			if (resp >= 0) {//if we require more controllers
				SDLController s;
				s.gameController = SDL_GameControllerOpen(e.cdevice.which);
				if (s.gameController == NULL) {
				}
				s.id=resp;
				SDL_Joystick* i=SDL_GameControllerGetJoystick(s.gameController);

				controllers[SDL_JoystickInstanceID(i)] = s;

			}
		}break;
		case SDL_CONTROLLERDEVICEREMOVED: {
			eventHandler.onControllerRemoved(controllers[e.cdevice.which].id);
			controllers.erase(e.cdevice.which);
		}break;

		case SDL_CONTROLLERBUTTONDOWN:
			eventHandler.onControllerDown(controllers[e.cdevice.which].id,e.cbutton.button);
		break;
		case SDL_CONTROLLERBUTTONUP:
			eventHandler.onControllerUp(controllers[e.cdevice.which].id,e.cbutton.button);
		break;
		case SDL_CONTROLLERAXISMOTION:
				eventHandler.onControllerAxis(controllers[e.cdevice.which].id,e.caxis.axis,e.caxis.value);
		break;
		default:
		break;
		};
	}
}

bool SDLApplication::isRunning()
{
	return isAppRunning;
}

