#include "interactionWorld.hpp"

void InteractionWorld::onMakeEntity(EntityHandle handle)
{
	entities.push_back(handle);
}

void InteractionWorld::onRemoveEntity(EntityHandle handle)
{
	entitiesToRemove.push_back(handle);
}

void InteractionWorld::onAddComponent(EntityHandle handle, uint32 id)
{
	if(id == TransformComponent::ID) {
		if(ecs.getComponent<ColliderComponent>(handle) != nullptr) {
			entities.push_back(handle);
		}
	}
	if(id == ColliderComponent::ID) {
		if(ecs.getComponent<TransformComponent>(handle) != nullptr) {
			entities.push_back(handle);
		}
	}
}

void InteractionWorld::onRemoveComponent(EntityHandle handle, uint32 id)
{
	if(id == TransformComponent::ID || id == ColliderComponent::ID) {
		entitiesToRemove.push_back(handle);
	}
}
