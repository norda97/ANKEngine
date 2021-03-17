#pragma once

#include "Core/Math/AnkMath.h"

struct RigidBody
{
	ANK_REG_COMP_INTERFACE(RigidBody);

	ANK_REG_VAR_INTERFACE(Vec3, Velocity);
	ANK_REG_VAR_INTERFACE(Vec3, Acceleration);
};