#include "ecs.hpp"
#include "core/memory.hpp"
#include "math/math.hpp"

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

EntityHandle ECS::makeEntity(BaseECSComponent** entityComponents, const uint32* componentIDs, size_t numComponents)
{
	std::pair<uint32, Array<std::pair<uint32, uint32> > >* newEntity = new std::pair<uint32, Array<std::pair<uint32, uint32> > >();
	EntityHandle handle = (EntityHandle)newEntity;
	for(uint32 i = 0; i < numComponents; i++) {
		if(!BaseECSComponent::isTypeValid(componentIDs[i])) {
			DEBUG_LOG("ECS", LOG_ERROR, "'%u' is not a valid component type.", componentIDs[i]);
			delete newEntity;
			return NULL_ENTITY_HANDLE;
		}

		addComponentInternal(handle, newEntity->second, componentIDs[i], entityComponents[i]);
	}

	newEntity->first = entities.size();
	entities.push_back(newEntity);

	for(uint32 i = 0; i < listeners.size(); i++) {
		bool isValid = true;
		for(uint32 j = 0; j < listeners[i]->getComponentIDs().size(); j++) {
			bool hasComponent = false;
			for(uint32 k = 0; k < numComponents; k++) {
				if(listeners[i]->getComponentIDs()[j] == componentIDs[k]) {
					hasComponent = true;
					break;
				}
			}
			if(!hasComponent) {
				isValid = false;
				break;
			}
		}
		if(isValid) {
			listeners[i]->onMakeEntity(handle);
		}
	}

	return handle;
}

void ECS::removeEntity(EntityHandle handle)
{
	Array<std::pair<uint32, uint32> >& entity = handleToEntity(handle);

	for(uint32 i = 0; i < listeners.size(); i++) {
		const Array<uint32>& componentIDs = listeners[i]->getComponentIDs();
		bool isValid = true;
		for(uint32 j = 0; j < componentIDs.size(); j++) {
			bool hasComponent = false;
			for(uint32 k = 0; k < entity.size(); k++) {
				if(componentIDs[j] == entity[k].first) {
					hasComponent = true;
					break;
				}
			}
			if(!hasComponent) {
				isValid = false;
				break;
			}
		}
		if(isValid) {
			listeners[i]->onRemoveEntity(handle);
		}
	}
	
	for(uint32 i = 0; i < entity.size(); i++) {
		deleteComponent(entity[i].first, entity[i].second);
	}

	uint32 destIndex = handleToEntityIndex(handle);
	uint32 srcIndex = entities.size() - 1;
	delete entities[destIndex];
	entities[destIndex] = entities[srcIndex];
	entities[destIndex]->first = destIndex;
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

BaseECSComponent* ECS::getComponentInternal(Array<std::pair<uint32, uint32> >& entityComponents, Array<uint8>& array, uint32 componentID)
{
	for(uint32 i = 0; i < entityComponents.size(); i++) {
		if(componentID == entityComponents[i].first) {
			return (BaseECSComponent*)&array[entityComponents[i].second];
		}
	}
	return nullptr;
}

void ECS::updateSystems(ECSSystemList& systems, float delta)
{
	Array<BaseECSComponent*> componentParam;
	Array<Array<uint8>*> componentArrays;
	for(uint32 i = 0; i < systems.size(); i++) {
		const Array<uint32>& componentTypes = systems[i]->getComponentTypes();
		if(componentTypes.size() == 1) {
			size_t typeSize = BaseECSComponent::getTypeSize(componentTypes[0]);
			Array<uint8>& array = components[componentTypes[0]];
			for(uint32 j = 0; j < array.size(); j += typeSize) {
				BaseECSComponent* component = (BaseECSComponent*)&array[j];
				systems[i]->updateComponents(delta, &component);
			}
		} else {
			updateSystemWithMultipleComponents(i, systems, delta, componentTypes, componentParam, componentArrays);
		}
	}
}

uint32 ECS::findLeastCommonComponent(const Array<uint32>& componentTypes, const Array<uint32>& componentFlags)
{
	uint32 minSize = (uint32)-1;
	uint32 minIndex = (uint32)-1;
	for(uint32 i = 0; i < componentTypes.size(); i++) {
		if((componentFlags[i] & BaseECSSystem::FLAG_OPTIONAL) != 0) {
			continue;
		}
		size_t typeSize = BaseECSComponent::getTypeSize(componentTypes[i]);
		uint32 size = components[componentTypes[i]].size()/typeSize;
		if(size <= minSize) {
			minSize = size;
			minIndex = i;
		}
	}

	return minIndex;
}

void ECS::updateSystemWithMultipleComponents(uint32 index, ECSSystemList& systems, float delta,
		const Array<uint32>& componentTypes, Array<BaseECSComponent*>& componentParam,
		Array<Array<uint8>*>& componentArrays)
{
	const Array<uint32>& componentFlags = systems[index]->getComponentFlags();

	componentParam.resize(Math::max(componentParam.size(), componentTypes.size()));
	componentArrays.resize(Math::max(componentArrays.size(), componentTypes.size()));
	for(uint32 i = 0; i < componentTypes.size(); i++) {
		componentArrays[i] = &components[componentTypes[i]];
	}
	uint32 minSizeIndex = findLeastCommonComponent(componentTypes, componentFlags);

	size_t typeSize = BaseECSComponent::getTypeSize(componentTypes[minSizeIndex]);
	Array<uint8>& array = *componentArrays[minSizeIndex];
	for(uint32 i = 0; i < array.size(); i += typeSize) {
		componentParam[minSizeIndex] = (BaseECSComponent*)&array[i];
		Array<std::pair<uint32, uint32> >& entityComponents =
			handleToEntity(componentParam[minSizeIndex]->entity);

		bool isValid = true;
		for(uint32 j = 0; j < componentTypes.size(); j++) {
			if(j == minSizeIndex) {
				continue;
			}

			componentParam[j] = getComponentInternal(entityComponents,
					*componentArrays[j], componentTypes[j]);
			if(componentParam[j] == nullptr && (componentFlags[j] & BaseECSSystem::FLAG_OPTIONAL) == 0) {
				isValid = false;
				break;
			}
		}

		if(isValid) {
			systems[index]->updateComponents(delta, &componentParam[0]);
		}
	}
}

