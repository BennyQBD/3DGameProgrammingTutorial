#pragma once

#include "ecsComponent.hpp"
#include "ecsSystem.hpp"
#include "dataStructures/map.hpp"

class ECS
{
public:
	ECS() {}
	~ECS();

	// Entity methods
	EntityHandle makeEntity(BaseECSComponent* components, const uint32* componentIDs, size_t numComponents);
	void removeEntity(EntityHandle handle);

	// Component methods
	template<class Component>
	void addComponent(EntityHandle entity, Component* component);

	template<class Component>
	void removeComponent(EntityHandle entity);

	template<class Component>
	void getComponent(EntityHandle entity);

	// System methods
	inline void addSystem(BaseECSSystem& system)
	{
		systems.push_back(&system);
	}
	void updateSystems(float delta);
	void removeSystem(BaseECSSystem& system);
private:
	Array<BaseECSSystem*> systems;
	Map<uint32, Array<uint8>> components;
	Array<std::pair<uint32, Array<std::pair<uint32, uint32> > >* > entities;

	inline std::pair<uint32, Array<std::pair<uint32, uint32> > >* handleToRawType(EntityHandle handle)
	{
		(std::pair<uint32, Array<std::pair<uint32, uint32> > >*)handle;
	}

	inline uint32 handleToEntityIndex(EntityHandle handle)
	{
		return handleToRawType(handle)->first;
	}

	inline Array<std::pair<uint32, uint32> >& handleToEntity(EntityHandle handle)
	{
		return handleToRawType(handle)->second;
	}

	NULL_COPY_AND_ASSIGN(ECS);
};
