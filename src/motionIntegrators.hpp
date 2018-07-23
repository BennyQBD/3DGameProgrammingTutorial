#pragma once

#include "math/vector.hpp"

namespace MotionIntegrators
{
	inline void verlet(Vector3f& pos, Vector3f& velocity, const Vector3f& acceleration, float delta)
	{
		float halfDelta = delta * 0.5f;
		pos += velocity * halfDelta;
		velocity += acceleration * delta;
		pos += velocity * halfDelta;
	}

	inline void forestRuth(Vector3f& pos, Vector3f& velocity, const Vector3f& acceleration, float delta)
	{
		static const float frCoefficient = 1.0f/(2.0f-Math::pow(2.0f,1.0f/3.0f));
		static const float frComplement = 1.0f - 2.0f*frCoefficient;
		verlet(pos, velocity, acceleration, delta*frCoefficient);
		verlet(pos, velocity, acceleration, delta*frComplement);
		verlet(pos, velocity, acceleration, delta*frCoefficient);
	}

	inline void modifiedEuler(Vector3f& pos, Vector3f& velocity, const Vector3f& acceleration, float delta)
	{
		velocity += acceleration * delta;
		pos += velocity * delta;
	}
}
