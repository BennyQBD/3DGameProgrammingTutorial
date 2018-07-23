#pragma once

#include "ecs/ecs.hpp"
#include "utilComponents.hpp"
#include "motionIntegrators.hpp"

struct MotionComponent : public ECSComponent<MotionComponent>
{
	Vector3f velocity = Vector3f(0.0f, 0.0f, 0.0f);
	Vector3f acceleration = Vector3f(0.0f, 0.0f, 0.0f);
};

class MotionSystem : public BaseECSSystem
{
public:
	MotionSystem() : BaseECSSystem()
	{
		addComponentType(TransformComponent::ID);
		addComponentType(MotionComponent::ID);
	}

	virtual void updateComponents(float delta, BaseECSComponent** components)
	{
		TransformComponent* transform = (TransformComponent*)components[0];
		MotionComponent* motion = (MotionComponent*)components[1];

		Vector3f newPos = transform->transform.getTranslation();
		MotionIntegrators::forestRuth(newPos, motion->velocity, motion->acceleration, delta);
		transform->transform.setTranslation(newPos);
	}
};

