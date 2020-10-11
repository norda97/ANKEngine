#pragma once

#include "Core/Math/AnkMath.h"

struct alignas(16) MaterialProperties
{
	Vec4 albedo;
	float roughness;
	float metallicness;
	float specular;
};
