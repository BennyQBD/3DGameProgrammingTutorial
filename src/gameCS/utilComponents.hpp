#pragma once

#include "ecs/ecs.hpp"
#include "math/transform.hpp"

struct TransformComponent : public ECSComponent<TransformComponent>
{
	Transform transform;
};

