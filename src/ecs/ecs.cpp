#include "ecs.hpp"
#include "core/memory.hpp"

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

		addComponentInternal(handle, newEntity->second, componentIDs[i], &entityComponents[i]);
	}

	newEntity->first = entities.size();
	entities.push_back(newEntity);
	return handle;
}

void ECS::removeEntity(EntityHandle handle)
{
	Array<std::pair<uint32, uint32> >& entity = handleToEntity(handle);
	for(uint32 i = 0; i < entity.size(); i++) {
		deleteComponent(entity[i].first, entity[i].second);
	}

	uint32 destIndex = handleToEntityIndex(handle);
	uint32 srcIndex = entities.size() - 1;
	delete entities[destIndex];
	entities[destIndex] = entities[srcIndex];
	entities.pop_back();
}

void ECS::addComponentInternal(EntityHandle handle, Array<std::pair<uint32, uint32> >& entity, uint32 componentID, BaseECSComponent* component)
{
	ECSComponentCreateFunction createfn = BaseECSComponent::getTypeCreateFunction(componentID);
	std::pair<uint32, uint32> newPair;
	newPair.first = componentID;
	newPair.second = createfn(components[componentID], handle, component);
	entity.push_back(newPair);
}

void ECS::deleteComponent(uint32 componentID, uint32 index)
{
	Array<uint8>& array = components[componentID];
	ECSComponentFreeFunction freefn = BaseECSComponent::getTypeFreeFunction(componentID);
	size_t typeSize = BaseECSComponent::getTypeSize(componentID);
	uint32 srcIndex = array.size() - typeSize;

	BaseECSComponent* destComponent = (BaseECSComponent*)&array[index];
	BaseECSComponent* srcComponent = (BaseECSComponent*)&array[srcIndex];
	freefn(destComponent);

	if(index == srcIndex) {
		array.resize(srcIndex);
		return;
	}
	Memory::memcpy(destComponent, srcComponent, typeSize);

	Array<std::pair<uint32, uint32> >& srcComponents = handleToEntity(srcComponent->entity);
	for(uint32 i = 0; i < srcComponents.size(); i++) {
		if(componentID == srcComponents[i].first && srcIndex == srcComponents[i].second) {
			srcComponents[i].second = index;
			break;
		}
	}

	array.resize(srcIndex);
}

bool ECS::removeComponentInternal(EntityHandle handle, uint32 componentID)
{
	Array<std::pair<uint32, uint32> >& entityComponents = handleToEntity(handle);
	for(uint32 i = 0; i < entityComponents.size(); i++) {
		if(componentID == entityComponents[i].first) {
			deleteComponent(entityComponents[i].first, entityComponents[i].second);
			uint32 srcIndex = entityComponents.size()-1;
			uint32 destIndex = i;
			entityComponents[destIndex] = entityComponents[srcIndex];
			entityComponents.pop_back();
			return true;
		}
	}
	return false;
}

BaseECSComponent* ECS::getComponentInternal(Array<std::pair<uint32, uint32> >& entityComponents, uint32 componentID)
{
	for(uint32 i = 0; i < entityComponents.size(); i++) {
		if(componentID == entityComponents[i].first) {
			return (BaseECSComponent*)&components[componentID][entityComponents[i].second];
		}
	}
	return nullptr;
}

