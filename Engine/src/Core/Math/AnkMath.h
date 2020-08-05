#pragma once


#ifdef ANK_DX11
using Vec4 = Vector4;
using Vec3 = Vector3;
using Vec2 = Vector2;
using Mat4 = Matrix;
#else
#include "Vec4.h"
#include "Vec3.h"
#include "Vec2.h"
#endif