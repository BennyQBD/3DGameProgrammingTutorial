#pragma once

#include "ecsComponent.hpp"
#include "ecsSystem.hpp"
#include "dataStructures/map.hpp"

class ECSListener
{
public:
	virtual void onMakeEntity(EntityHandle handle) {}
	virtual void onRemoveEntity(EntityHandle handle) {}
	virtual void onAddComponent(EntityHandle handle, uint32 id) {}
	virtual void onRemoveComponent(EntityHandle handle, uint32 id) {}

	const Array<uint32>& getComponentIDs() { 
		return componentIDs;
	}

	inline bool shouldNotifyOnAllComponentOperations() {
		return notifyOnAllComponentOperations;
	}
	inline bool shouldNotifyOnAllEntityOperations() {
		return notifyOnAllEntityOperations;
	}

protected:
	void setNotificationSettings(
			bool shouldNotifyOnAllComponentOperations,
			bool shouldNotifyOnAllEntityOperations) {
		notifyOnAllComponentOperations = shouldNotifyOnAllComponentOperations;
		notifyOnAllEntityOperations = shouldNotifyOnAllEntityOperations;
	}
	void addComponentID(uint32 id) {
		componentIDs.push_back(id);
	}
private:
	Array<uint32> componentIDs;
	bool notifyOnAllComponentOperations = false;
	bool notifyOnAllEntityOperations = false;
};

class ECS
{
public:
	ECS() {}
	~ECS();

	// ECSListener methods
	inline void addListener(ECSListener* listener) {
		listeners.push_back(listener);
	}

	// Entity methods
	EntityHandle makeEntity(BaseECSComponent** components, const uint32* componentIDs, size_t numComponents);
	void removeEntity(EntityHandle handle);

	template<class... Components>
	EntityHandle makeEntity(Components&&... entitycomponents)
	{
		BaseECSComponent* components[] = { (&entitycomponents)... };
		uint32 componentIDs[] = { (std::remove_reference_t<Components>::ID)... };
		return makeEntity(components, componentIDs, sizeof...(Components));
	}

	// Component methods
	template<class Component>
	inline void addComponent(EntityHandle entity, Component* component)
	{
		addComponentInternal(entity, handleToEntity(entity), Component::ID, component);
		for(uint32 i = 0; i < listeners.size(); i++) {
			const Array<uint32>& componentIDs = listeners[i]->getComponentIDs();
			if(listeners[i]->shouldNotifyOnAllComponentOperations()) {
				listeners[i]->onAddComponent(entity, Component::ID);
			} else {
				for(uint32 j = 0; j < componentIDs.size(); j++) {
					if(componentIDs[j] == Component::ID) {
						listeners[i]->onAddComponent(entity, Component::ID);
						break;
					}
				}
			}
		}
	}

	template<class Component>
	bool removeComponent(EntityHandle entity)
	{
		for(uint32 i = 0; i < listeners.size(); i++) {
			const Array<uint32>& componentIDs = listeners[i]->getComponentIDs();
			for(uint32 j = 0; j < componentIDs.size(); j++) {
				if(listeners[i]->shouldNotifyOnAllComponentOperations()) {
					listeners[i]->onRemoveComponent(entity, Component::ID);
				} else {
					if(componentIDs[j] == Component::ID) {
						listeners[i]->onRemoveComponent(entity, Component::ID);
						break;
					}
				}
			}
		}
		return removeComponentInternal(entity, Component::ID);
	}

	template<class Component>
	Component* getComponent(EntityHandle entity)
	{
		return (Component*)getComponentInternal(handleToEntity(entity), components[Component::ID], Component::ID);
	}

	BaseECSComponent* getComponentByType(EntityHandle entity, uint32 componentID)
	{
		return getComponentInternal(handleToEntity(entity), components[componentID], componentID);
	}

	// System methods
	void updateSystems(ECSSystemList& systems, float delta);
	
private:
	Map<uint32, Array<uint8>> components;
	Array<std::pair<uint32, Array<std::pair<uint32, uint32> > >* > entities;
	Array<ECSListener*> listeners;

	inline std::pair<uint32, Array<std::pair<uint32, uint32> > >* handleToRawType(EntityHandle handle)
	{
		return (std::pair<uint32, Array<std::pair<uint32, uint32> > >*)handle;
	}

	inline uint32 handleToEntityIndex(EntityHandle handle)
	{
		return handleToRawType(handle)->first;
	}

	inline Array<std::pair<uint32, uint32> >& handleToEntity(EntityHandle handle)
	{
		return handleToRawType(handle)->second;
	}

	void deleteComponent(uint32 componentID, uint32 index);
	bool removeComponentInternal(EntityHandle handle, uint32 componentID);
	void addComponentInternal(EntityHandle handle, Array<std::pair<uint32, uint32> >& entity, uint32 componentID, BaseECSComponent* component);
	BaseECSComponent* getComponentInternal(Array<std::pair<uint32, uint32> >& entityComponents, Array<uint8>& array, uint32 componentID);

	void updateSystemWithMultipleComponents(uint32 index, ECSSystemList& systems, float delta, const Array<uint32>& componentTypes,
			Array<BaseECSComponent*>& componentParam, Array<Array<uint8>*>& componentArrays);
	uint32 findLeastCommonComponent(const Array<uint32>& componentTypes, const Array<uint32>& componentFlags);

	NULL_COPY_AND_ASSIGN(ECS);
};
