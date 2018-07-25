#include "interactionWorld.hpp"
#include <algorithm>

void InteractionWorld::onMakeEntity(EntityHandle handle)
{
	addEntity(handle);
}

void InteractionWorld::onRemoveEntity(EntityHandle handle)
{
	entitiesToRemove.push_back(handle);
}

void InteractionWorld::onAddComponent(EntityHandle handle, uint32 id)
{
	if(id == TransformComponent::ID) {
		if(ecs.getComponent<ColliderComponent>(handle) != nullptr) {
			addEntity(handle);
		}
	}
	if(id == ColliderComponent::ID) {
		if(ecs.getComponent<TransformComponent>(handle) != nullptr) {
			addEntity(handle);
		}
	}
}

void InteractionWorld::onRemoveComponent(EntityHandle handle, uint32 id)
{
	if(id == TransformComponent::ID || id == ColliderComponent::ID) {
		entitiesToRemove.push_back(handle);
	}
}

void InteractionWorld::addEntity(EntityHandle handle)
{
	EntityInternal entity;
	entity.handle = handle;
	// TODO: Compute interactions!
	entities.push_back(entity);
}

void InteractionWorld::computeInteractions(EntityInternal& entity, uint32 interactionIndex)
{
	Interaction* interaction = interactions[interactionIndex];
	bool isInteractor = true;
	for(size_t i = 0; i < interaction->getInteractorComponents().size(); i++) {
		if(ecs.getComponentByType(entity.handle, interaction->getInteractorComponents()[i]) == nullptr) {
			isInteractor = false;
			break;
		}
	}
	bool isInteractee = true;
	for(size_t i = 0; i < interaction->getInteracteeComponents().size(); i++) {
		if(ecs.getComponentByType(entity.handle, interaction->getInteracteeComponents()[i]) == nullptr) {
			isInteractee = false;
			break;
		}
	}
	if(isInteractor) {
		entity.interactors.push_back(interactionIndex);
	}
	if(isInteractee) {
		entity.interactees.push_back(interactionIndex);
	}
}

void InteractionWorld::processInteractions(float delta)
{
	removeEntities();
	std::sort(entities.begin(), entities.end(), compareAABB);
	// Go through the list, test intersections in range
	Vector3f centerSum(0.0f);
	Vector3f centerSqSum(0.0f);
	for(size_t i = 0; i < entities.size(); i++) {
		AABB aabb = ecs.getComponent<ColliderComponent>(entities[i].handle)->aabb;
		Vector3f center = aabb.getCenter();
		centerSum += center;
		centerSqSum += (center * center);
		// Find intersections for this entity
		for(size_t j = i-1; j < entities.size(); j++) {
			AABB otherAABB = ecs.getComponent<ColliderComponent>(entities[j].handle)->aabb;
			if(otherAABB.getMinExtents()[compareAABB.axis]
					> aabb.getMaxExtents()[compareAABB.axis]) {
				break;
			}

			if(aabb.intersects(otherAABB)) {
				// if rules say so, then entities[i] interacts with entities[j]
				// if rules say so, then entities[j] interacts with entities[i]
			}
		}
	}

	// Set max variance axis
	centerSum /= entities.size();
	centerSqSum /= entities.size();
	Vector3f variance = centerSqSum - (centerSum*centerSum);
	float maxVar = variance[0];
	uint32 maxVarAxis = 0;
	if(variance[1] > maxVar) {
		maxVar = variance[1];
		maxVarAxis = 1;
	}
	if(variance[2] > maxVar) {
		maxVar = variance[2];
		maxVarAxis = 2;
	}
	compareAABB.axis = maxVarAxis;
}

void InteractionWorld::removeEntities()
{
	if(entitiesToRemove.size() == 0) {
		return;
	}
	for(size_t i = 0; i < entities.size(); i++) {
		bool didRemove = false;
		do {
			didRemove = false;
			for(size_t j = 0; j < entitiesToRemove.size(); j++) {
				if(entities[i].handle == entitiesToRemove[j]) {
					entities.swap_remove(i);
					entitiesToRemove.swap_remove(j);
					didRemove = true;
					break;
				}
			}
			if(didRemove && entitiesToRemove.size() == 0) {
				return;
			}
		} while(didRemove);
	}
	entitiesToRemove.clear();
}

