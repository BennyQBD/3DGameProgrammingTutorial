#include "ecsComponent.hpp"

static uint32 componentID = 0;

uint32 BaseECSComponent::nextID()
{
	return componentID++;
}
