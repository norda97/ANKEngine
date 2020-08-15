#pragma once

#include "Core/Model/ModelTypes.h"

struct Drawable
{
	Vector4 albedo;
	float roughness;
	float metallicness;
	float specular;
	ModelID modelID;
};