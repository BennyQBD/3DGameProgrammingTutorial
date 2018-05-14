#pragma once

#include "core/iapplicationEventHandler.hpp"
#include "dataStructures/map.hpp"
#include "dataStructures/array.hpp"
#include "inputControl.hpp"

class GameEventHandler : public IApplicationEventHandler
{
public:
	GameEventHandler() {}
	virtual ~GameEventHandler() {}
	virtual void onKeyDown(uint32 keyCode, bool isRepeat);
	virtual void onKeyUp(uint32 keyCode, bool isRepeat);
	virtual void onMouseDown(uint32 mouseButton, uint8 numClicks);
	virtual void onMouseUp(uint32 mouseButton, uint8 numClicks);
	virtual void onMouseMove(int32 mousePosX, int32 mousePosY, 
			int32 deltaX, int32 deltaY);

	void addKeyControl(uint32 keyCode, InputControl& inputControl, float weight = 1.0f);
	void addMouseControl(uint32 mouseButton, InputControl& inputControl, float weight = 1.0f);
private:
	Map<uint32, Array<std::pair<float, InputControl&> > > inputs;
	void updateInput(uint32 inputCode, float dir, bool isRepeat);
	NULL_COPY_AND_ASSIGN(GameEventHandler);
};
