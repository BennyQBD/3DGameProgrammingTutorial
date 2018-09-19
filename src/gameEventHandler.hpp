#pragma once

#include "core/iapplicationEventHandler.hpp"
#include "dataStructures/map.hpp"
#include "dataStructures/array.hpp"
#include "inputControl.hpp"

class GameEventHandler : public IApplicationEventHandler
{
	struct Controller{
		uint32 id;
		bool connected;
		Map<uint8, Array<std::pair<float, InputControl&> > > buttons;
		struct Axis{
			Array<std::pair<float, InputControl&> > inputs;
			int16 lastVal;
		};
		Map<uint8, Axis > axis;
		Controller(){
			connected=false;
		}
	};
public:
	GameEventHandler() {}
	virtual ~GameEventHandler() {}
	virtual void onKeyDown(uint32 keyCode, bool isRepeat);
	virtual void onKeyUp(uint32 keyCode, bool isRepeat);
	virtual void onMouseDown(uint32 mouseButton, uint8 numClicks);
	virtual void onMouseUp(uint32 mouseButton, uint8 numClicks);
	virtual void onMouseMove(int32 mousePosX, int32 mousePosY, 
			int32 deltaX, int32 deltaY);

	virtual int32 onControllerAdded();
	virtual void onControllerRemoved(int32 controllerID);
	virtual void onControllerUp(int32 controllerID,uint8 controllerButton);
	virtual void onControllerDown(int32 controllerID,uint8 controllerButton);
	virtual void onControllerAxis(int32 controllerID,uint8 controllerAxis, int16 axisValue);

	void addKeyControl(uint32 keyCode, InputControl& inputControl, float weight = 1.0f);
	void addMouseControl(uint32 mouseButton, InputControl& inputControl, float weight = 1.0f);

	void addController(uint32 controllerID);
	void addControllerButtonControl(uint32 controllerID,uint8 button, InputControl& inputControl, float weight = 1.0f);
	void addControllerAxisControl(uint32 controllerID,uint8 axis, InputControl& inputControl, float weight = 1.0f);

private:
	Map<uint32, Array<std::pair<float, InputControl&> > > inputs;
	Map<uint32,Controller> controllers;
	void updateInput(uint32 inputCode, float dir, bool isRepeat);
	void updateControllerButton(int32 controller,uint8 button,float dir);
	NULL_COPY_AND_ASSIGN(GameEventHandler);
};
