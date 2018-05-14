#pragma once

#include "core/common.hpp"
#include "core/iapplicationEventHandler.hpp"

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

	SDLApplication();
};
