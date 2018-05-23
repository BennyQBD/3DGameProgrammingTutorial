#include "ecs.hpp"

ECS::~ECS()
{
	for(Map<uint32, Array<uint8>>::iterator it = components.begin(); it != components.end(); ++it) {
		size_t typeSize = BaseECSComponent::getTypeSize(it->first);
		ECSComponentFreeFunction freefn = BaseECSComponent::getTypeFreeFunction(it->first);
		for(uint32 i = 0; i < it->second.size(); i += typeSize) {
			freefn((BaseECSComponent*)&it->second[i]);
		}
	}

	for(uint32 i = 0; i < entities.size(); i++) {
		delete entities[i];
	}
}

EntityHandle ECS::makeEntity(BaseECSComponent* entityComponents, const uint32* componentIDs, size_t numComponents)
{
	std::pair<uint32, Array<std::pair<uint32, uint32> > >* newEntity = new std::pair<uint32, Array<std::pair<uint32, uint32> > >();
	EntityHandle handle = (EntityHandle)newEntity;
	for(uint32 i = 0; i < numComponents; i++) {
		if(!BaseECSComponent::isTypeValid(componentIDs[i])) {
			DEBUG_LOG("ECS", LOG_ERROR, "'%u' is not a valid component type.", componentIDs[i]);
			delete newEntity;
			return NULL_ENTITY_HANDLE;
		}

		ECSComponentCreateFunction createfn = BaseECSComponent::getTypeCreateFunction(componentIDs[i]);
		std::pair<uint32, uint32> newPair;
		newPair.first = componentIDs[i];
		newPair.second = createfn(components[componentIDs[i]], handle, &entityComponents[i]);
		newEntity->second.push_back(newPair);
	}

	newEntity->first = entities.size();
	entities.push_back(newEntity);
	return handle;
}

void ECS::removeEntity(EntityHandle handle)
{
	Array<std::pair<uint32, uint32> >& entity = handleToEntity(handle);
	for(uint32 i = 0; i < entity.size(); i++) {
		removeComponentInternal(entity[i].first, entity[i].second);
	}

	uint32 destIndex = handleToEntityIndex(handle);
	uint32 srcIndex = entities.size() - 1;
	delete entities[destIndex];
	entities[destIndex] = entities[srcIndex];
	entities.pop_back();
}
