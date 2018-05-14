#pragma once

#include "math/math.hpp"

class InputControl
{
public:
	InputControl();
	void addAmt(float amtToAdd);
	float getAmt();
private:
	float amt;
};

inline InputControl::InputControl() :
	amt(0.0f) {}

inline void InputControl::addAmt(float amtToAdd)
{
	amt += amtToAdd;
}

inline float InputControl::getAmt()
{
	return Math::clamp(amt, -1.0f, 1.0f);
}
