#pragma once

#include "ecs/ecs.hpp"
#include "utilComponents.hpp"
#include "gameRenderContext.hpp"

struct RenderableMeshComponent : public ECSComponent<RenderableMeshComponent>
{
	VertexArray* vertexArray = nullptr;
	Texture* texture = nullptr;
};

class RenderableMeshSystem : public BaseECSSystem
{
public:
	RenderableMeshSystem(GameRenderContext& contextIn) : BaseECSSystem(), context(contextIn)
	{
		addComponentType(TransformComponent::ID);
		addComponentType(RenderableMeshComponent::ID);
	}

	virtual void updateComponents(float delta, BaseECSComponent** components)
	{
		TransformComponent* transform = (TransformComponent*)components[0];
		RenderableMeshComponent* mesh = (RenderableMeshComponent*)components[1];

		context.renderMesh(*mesh->vertexArray, *mesh->texture, transform->transform.toMatrix());
	}
private:
	GameRenderContext& context;
};


