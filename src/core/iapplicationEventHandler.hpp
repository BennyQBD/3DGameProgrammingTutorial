#pragma once

#include "common.hpp"

class IApplicationEventHandler
{
public:
	IApplicationEventHandler() {}
	virtual ~IApplicationEventHandler() {}
	virtual void onKeyDown(uint32 keyCode, bool isRepeat) {}
	virtual void onKeyUp(uint32 keyCode, bool isRepeat) {}
	virtual void onMouseDown(uint32 mouseButton, uint8 numClicks) {}
	virtual void onMouseUp(uint32 mouseButton, uint8 numClicks) {}
	virtual void onMouseMove(int32 mousePosX, int32 mousePosY, 
			int32 deltaX, int32 deltaY) {}

	virtual int32 onControllerAdded(){}
	virtual void onControllerRemoved(int32 controllerID){}
	virtual void onControllerDown(int32 controllerID,uint8 controllerButton){}
	virtual void onControllerUp(int32 controllerID,uint8 controllerButton){}
	virtual void onControllerAxis(int32 controllerID,uint8 controllerAxis, int16 axisValue){}


private:
	NULL_COPY_AND_ASSIGN(IApplicationEventHandler);
};
