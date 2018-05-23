#include "ecsComponent.hpp"

Array<std::tuple<ECSComponentCreateFunction, ECSComponentFreeFunction, size_t> > BaseECSComponent::componentTypes;

uint32 BaseECSComponent::registerComponentType(ECSComponentCreateFunction createfn,
			ECSComponentFreeFunction freefn, size_t size)
{
	uint32 componentID = componentTypes.size();
	componentTypes.push_back(std::tuple<ECSComponentCreateFunction, ECSComponentFreeFunction, size_t>(
				createfn, freefn, size));
	return componentID;
}
