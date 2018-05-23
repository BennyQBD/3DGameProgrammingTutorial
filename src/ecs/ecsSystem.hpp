#pragma once

#include "ecsComponent.hpp"

class BaseECSSystem
{
public:
	BaseECSSystem(const Array<uint32>& componentTypesIn) : componentTypes(componentTypesIn) {}
	virtual void updateComponents(float delta, BaseECSComponent** components) {}
	const Array<uint32>& getComponentTypes()
	{
		return componentTypes;
	}
private:
	Array<uint32> componentTypes;
};
