#pragma once

#include "Core/Math/AnkMath.h"

#include "Core/Utils/InterfaceGfxDbg/AnkDebugInterfaceInclude.h"

struct Transform
{
	ANK_REG_COMP_INTERFACE(Transform);

	ANK_REG_VAR_INTERFACE(Vec3, Position);
	ANK_REG_VAR_INTERFACE(Vec3, Rotation);
	ANK_REG_VAR_INTERFACE(Vec3, Scale);
};