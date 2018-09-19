#include "gameEventHandler.hpp"
#include "core/input.hpp"

#define MOUSE_OFFSET Input::NUM_KEYS

void GameEventHandler::onKeyDown(uint32 keyCode, bool isRepeat)
{
	updateInput(keyCode, 1.0f, isRepeat);
}

void GameEventHandler::onKeyUp(uint32 keyCode, bool isRepeat)
{
	updateInput(keyCode, -1.0f, isRepeat);
}

void GameEventHandler::onMouseDown(uint32 mouseButton, uint8 numClicks)
{
	updateInput(mouseButton+MOUSE_OFFSET, 1.0f, false);
}

void GameEventHandler::onMouseUp(uint32 mouseButton, uint8 numClicks)
{
	updateInput(mouseButton+MOUSE_OFFSET, -1.0f, false);
}

void GameEventHandler::onMouseMove(int32 mousePosX, int32 mousePosY, 
		int32 deltaX, int32 deltaY) {}

void GameEventHandler::addKeyControl(uint32 keyCode, InputControl& inputControl, float weight)
{
	inputs[keyCode].push_back(std::pair<float, InputControl&>(weight, inputControl));
}
void GameEventHandler::addMouseControl(uint32 mouseButton, InputControl& inputControl, float weight)
{
	inputs[mouseButton+MOUSE_OFFSET].push_back(std::pair<float, InputControl&>(weight, inputControl));
}

void GameEventHandler::updateInput(uint32 inputCode, float dir, bool isRepeat)
{
	if(isRepeat) {
		return;
	}

	for(uint32 i = 0; i < inputs[inputCode].size(); i++) {
		inputs[inputCode][i].second.addAmt(inputs[inputCode][i].first * dir);
	}
}

void GameEventHandler::addController(uint32 controllerID) {
	controllers[controllerID].id=controllerID;
}

int32 GameEventHandler::onControllerAdded() {
	int32 o=-1;
	for(auto& c:controllers)
		if(!c.second.connected){
			c.second.connected=true;
			return c.second.id;
		}
	return o;
}

void GameEventHandler::onControllerRemoved(int32 controllerID) {
	controllers[controllerID].connected=false;
}

void GameEventHandler::addControllerButtonControl(uint32 controllerID,uint8 button, InputControl &inputControl, float weight) {
	controllers[controllerID].buttons[button].push_back(std::pair<float, InputControl&>(weight, inputControl));
}

void
GameEventHandler::addControllerAxisControl(uint32 controllerID, uint8 axis, InputControl &inputControl, float weight) {
	controllers[controllerID].axis[axis].inputs.push_back(std::pair<float, InputControl&>(weight, inputControl));
	controllers[controllerID].axis[axis].lastVal=0;
}

void GameEventHandler::updateControllerButton(int32 controllerID,uint8 button, float dir) {
	for(uint32 i = 0; i < 	controllers[controllerID].buttons[button]
									 .size(); i++) {
		controllers[controllerID]
		.buttons[button][i]
		.second
		.addAmt(
				controllers[controllerID]
				.buttons[button][i]
				.first * dir);
	}
}

void GameEventHandler::onControllerUp(int32 controllerID, uint8 controllerButton) {
	updateControllerButton(controllerID,controllerButton,1.0f);
}

void GameEventHandler::onControllerDown(int32 controllerID, uint8 controllerButton) {
	updateControllerButton(controllerID,controllerButton,-1.0f);
}

void GameEventHandler::onControllerAxis(int32 controllerID, uint8 controllerAxis, int16 axisValue) {
	int16 v=controllers[controllerID].axis[controllerAxis].lastVal;
	float w=((float)(axisValue-v)+0.5f)/(32767.5f);
	for(uint32 i = 0; i < controllers[controllerID].axis[controllerAxis].inputs.size(); i++){
		controllers[controllerID].axis[controllerAxis].inputs[i].second.addAmt(
				controllers[controllerID].axis[controllerAxis].inputs[i].first*w
				);
	}

	controllers[controllerID].axis[controllerAxis].lastVal=axisValue;
}
