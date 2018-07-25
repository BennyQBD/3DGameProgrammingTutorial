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
protected:
	void addComponentID(uint32 id) {
		componentIDs.push_back(id);
	}
private:
	Array<uint32> componentIDs;
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

	template<class A>
	EntityHandle makeEntity(A& c1)
	{
		BaseECSComponent* components[] = { &c1 };
		uint32 componentIDs[] = {A::ID};
		return makeEntity(components, componentIDs, 1);
	}

	template<class A, class B>
	EntityHandle makeEntity(A& c1, B& c2)
	{
		BaseECSComponent* components[] = { &c1, &c2 };
		uint32 componentIDs[] = {A::ID, B::ID};
		return makeEntity(components, componentIDs, 2);
	}

	template<class A, class B, class C>
	EntityHandle makeEntity(A& c1, B& c2, C& c3)
	{
		BaseECSComponent* components[] = { &c1, &c2, &c3 };
		uint32 componentIDs[] = {A::ID, B::ID, C::ID};
		return makeEntity(components, componentIDs, 3);
	}

	template<class A, class B, class C, class D>
	EntityHandle makeEntity(A& c1, B& c2, C& c3, D& c4)
	{
		BaseECSComponent* components[] = { &c1, &c2, &c3, &c4 };
		uint32 componentIDs[] = {A::ID, B::ID, C::ID, D::ID};
		return makeEntity(components, componentIDs, 4);
	}

	template<class A, class B, class C, class D, class E>
	EntityHandle makeEntity(A& c1, B& c2, C& c3, D& c4, E& c5)
	{
		BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5 };
		uint32 componentIDs[] = {A::ID, B::ID, C::ID, D::ID, E::ID};
		return makeEntity(components, componentIDs, 5);
	}

	template<class A, class B, class C, class D, class E, class F>
	EntityHandle makeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6)
	{
		BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6};
		uint32 componentIDs[] = {A::ID, B::ID, C::ID, D::ID, E::ID, F::ID};
		return makeEntity(components, componentIDs, 6);
	}

	template<class A, class B, class C, class D, class E, class F, class G>
	EntityHandle makeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6, G& c7)
	{
		BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6, &c7};
		uint32 componentIDs[] = {A::ID, B::ID, C::ID, D::ID, E::ID, F::ID, G::ID};
		return makeEntity(components, componentIDs, 7);
	}

	template<class A, class B, class C, class D, class E, class F, class G, class H>
	EntityHandle makeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6, G& c7, H& c8)
	{
		BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6, &c7, &c8};
		uint32 componentIDs[] = {A::ID, B::ID, C::ID, D::ID, E::ID, F::ID, G::ID, H::ID};
		return makeEntity(components, componentIDs, 8);
	}

	template<class A, class B, class C, class D, class E, class F, class G, class H, class I>
	EntityHandle makeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6, G& c7, H& c8, I& c9)
	{
		BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6, &c7, &c8, &c9 };
		uint32 componentIDs[] = {A::ID, B::ID, C::ID, D::ID, E::ID, F::ID, G::ID, H::ID, I::ID};
		return makeEntity(components, componentIDs, 9);
	}

	template<class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
	EntityHandle makeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6, G& c7, H& c8, I& c9, J& c10)
	{
		BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6, &c7, &c8, &c9, &c10 };
		uint32 componentIDs[] = {A::ID, B::ID, C::ID, D::ID, E::ID, F::ID, G::ID, H::ID, I::ID, J::ID };
		return makeEntity(components, componentIDs, 10);
	}



	// Component methods
	template<class Component>
	inline void addComponent(EntityHandle entity, Component* component)
	{
		addComponentInternal(entity, handleToEntity(entity), Component::ID, component);
		for(uint32 i = 0; i < listeners.size(); i++) {
			const Array<uint32>& componentIDs = listeners[i]->getComponentIDs();
			for(uint32 j = 0; j < componentIDs.size(); j++) {
				if(componentIDs[j] == Component::ID) {
					listeners[i]->onAddComponent(entity, Component::ID);
					break;
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
				if(componentIDs[j] == Component::ID) {
					listeners[i]->onRemoveComponent(entity, Component::ID);
					break;
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
