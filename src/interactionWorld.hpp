#pragma once

#include "ecs/ecs.hpp"
#include "gameCS/utilComponents.hpp"

class Interaction
{
public:
	virtual void interact(float delta, BaseECSComponent** interactorComponents, BaseECSComponent** interacteeComponents) {}

	const Array<uint32>& getInteractorComponents() {
		return interactorComponentTypes;
	}
	const Array<uint32>& getInteracteeComponents() {
		return interacteeComponentTypes;
	}
protected:
	void addInteractorComponentType(uint32 type) {
		interactorComponentTypes.push_back(type);
	}
	void addInteracteeComponentType(uint32 type) {
		interacteeComponentTypes.push_back(type);
	}
private:
	Array<uint32> interactorComponentTypes;
	Array<uint32> interacteeComponentTypes;
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

	inline void addInteraction(Interaction* interaction) {
		interactions.push_back(interaction);
		// TODO: Update entities
	}
private:
	struct EntityInternal {
		EntityHandle handle;
		Array<uint32> interactors;
		Array<uint32> interactees;
	};

	struct InteractionWorldCompare {
		uint32 axis;
		ECS& ecs;

		InteractionWorldCompare(ECS& ecsIn, uint32 axisIn) :
			axis(axisIn), ecs(ecsIn) {}
		bool operator()(EntityInternal& a, EntityInternal& b) {
			float aMin = ecs.getComponent<ColliderComponent>(a.handle)->aabb.getMinExtents()[axis];
			float bMin = ecs.getComponent<ColliderComponent>(b.handle)->aabb.getMinExtents()[axis];
			return (aMin < bMin);
		}
	};

	Array<EntityInternal> entities;
	Array<EntityHandle> entitiesToRemove;
	Array<Interaction*> interactions;
	ECS& ecs;
	InteractionWorldCompare compareAABB;

	void removeEntities();
	void addEntity(EntityHandle handle);

	void computeInteractions(EntityInternal& entity, uint32 interactionIndex);
};
