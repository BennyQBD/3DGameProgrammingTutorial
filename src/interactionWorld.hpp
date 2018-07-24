#pragma once

#include "ecs/ecs.hpp"
#include "gameCS/utilComponents.hpp"

struct InteractionWorldCompare {
	uint32 axis;
	ECS& ecs;

	InteractionWorldCompare(ECS& ecsIn, uint32 axisIn) :
		axis(axisIn), ecs(ecsIn) {}
	bool operator()(EntityHandle a, EntityHandle b) {
		float aMin = ecs.getComponent<ColliderComponent>(a)->aabb.getMinExtents()[axis];
		float bMin = ecs.getComponent<ColliderComponent>(b)->aabb.getMinExtents()[axis];
		return (aMin < bMin);
	}
};

class InteractionWorld : public ECSListener
{
public:
	InteractionWorld(ECS& ecsIn) : ECSListener(), ecs(ecsIn), compareAABB(ecsIn, 0)
	{
		addComponentID(TransformComponent::ID);
		addComponentID(ColliderComponent::ID);
	}
	virtual void onMakeEntity(EntityHandle handle);
	virtual void onRemoveEntity(EntityHandle handle);
	virtual void onAddComponent(EntityHandle handle, uint32 id);
	virtual void onRemoveComponent(EntityHandle handle, uint32 id);

	void processInteractions(float delta);
private:
	Array<EntityHandle> entities;
	Array<EntityHandle> entitiesToRemove;
	ECS& ecs;
	InteractionWorldCompare compareAABB;

	void removeEntities();
};
