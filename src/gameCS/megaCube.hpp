#pragma once

#include "ecs/ecs.hpp"
#include "motionIntegrators.hpp"
#include "gameRenderContext.hpp"

struct MegaCubeComponent : public ECSComponent<MegaCubeComponent>
{
	float pos[3];
	float vel[3];
	float acc[3];
	uint8 texIndex;
};

class MegaCubeMotionSystem : public BaseECSSystem
{
public:
	MegaCubeMotionSystem() : BaseECSSystem()
	{
		addComponentType(MegaCubeComponent::ID);
	}

	virtual void updateComponents(float delta, BaseECSComponent** components)
	{
		MegaCubeComponent* comp = (MegaCubeComponent*)components[0];

		Vector3f newPos(comp->pos[0], comp->pos[1], comp->pos[2]);
		Vector3f newVel(comp->vel[0], comp->vel[1], comp->vel[2]);
		Vector3f acc(comp->acc[0], comp->acc[1], comp->acc[2]);
		MotionIntegrators::forestRuth(newPos, newVel, acc, delta);
		comp->pos[0] = newPos[0];
		comp->pos[1] = newPos[1];
		comp->pos[2] = newPos[2];
		comp->vel[0] = newVel[0];
		comp->vel[1] = newVel[1];
		comp->vel[2] = newVel[2];
	}
};

class MegaCubeRenderer : public BaseECSSystem
{
public:
	MegaCubeRenderer(GameRenderContext& contextIn, VertexArray& vertexArrayIn, Texture** texturesIn, size_t numTexturesIn)
		: BaseECSSystem(), context(contextIn),
		vertexArray(vertexArrayIn), textures(texturesIn), numTextures(numTexturesIn)
	{
		addComponentType(MegaCubeComponent::ID);
	}

	virtual void updateComponents(float delta, BaseECSComponent** components)
	{
		MegaCubeComponent* comp = (MegaCubeComponent*)components[0];

		Transform transform(Vector3f(comp->pos[0], comp->pos[1], comp->pos[2]));
		context.renderMesh(vertexArray, *(textures[comp->texIndex]), transform.toMatrix());
	}
private:
	GameRenderContext& context;
	VertexArray& vertexArray;
	Texture** textures;
	size_t numTextures;
};
