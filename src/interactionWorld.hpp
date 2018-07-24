#pragma once

#include "ecs/ecs.hpp"
#include "gameCS/utilComponents.hpp"

class InteractionWorld : public ECSListener
{
public:
	InteractionWorld(ECS& ecsIn) : ECSListener(), ecs(ecsIn)
	{
		addComponentID(TransformComponent::ID);
		addComponentID(ColliderComponent::ID);
	}
	virtual void onMakeEntity(EntityHandle handle);
	virtual void onRemoveEntity(EntityHandle handle);
	virtual void onAddComponent(EntityHandle handle, uint32 id);
	virtual void onRemoveComponent(EntityHandle handle, uint32 id);
private:
	Array<EntityHandle> entities;
	Array<EntityHandle> entitiesToRemove;
	ECS& ecs;
};
