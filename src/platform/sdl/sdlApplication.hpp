#pragma once

#include <SDL2/SDL_gamecontroller.h>
#include <dataStructures/map.hpp>
#include "core/common.hpp"
#include "core/iapplicationEventHandler.hpp"

struct SDLController{
	SDL_GameController* gameController;
	int32 id;
	SDLController(){
		id=0;
		gameController=nullptr;
	}

	SDLController(SDLController&& sdl){
		id=sdl.id;
		gameController=sdl.gameController;

		sdl.id=-1;
		sdl.gameController=nullptr;
	}
	~SDLController(){
		if(gameController!= nullptr){
		//	SDL_GameControllerClose(gameController);
			gameController=nullptr;
		}
	}

	SDLController& operator = (const SDLController& controller)= delete;

	SDLController& operator = (SDLController&& controller){
		gameController=controller.gameController;
		id=controller.id;

		controller.id=-1;
		controller.gameController=nullptr;
		return *this;
	}
};

class SDLApplication
{
public:
	static SDLApplication* create();
	
	virtual ~SDLApplication();
	virtual void processMessages(double delta, IApplicationEventHandler& eventHandler);
	virtual bool isRunning();
private:
	bool isAppRunning;
	static uint32 numInstances;
	std::map<SDL_JoystickID ,SDLController> controllers;
	SDLApplication();
};
