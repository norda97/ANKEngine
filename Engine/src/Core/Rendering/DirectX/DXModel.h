#pragma once

#include "DXMesh.h"

#include "Core/Model/Model.h"

class DXModel : public Model

{
public:
	DXModel();
	~DXModel();

	const std::vector<Mesh>& getMeshes() const;
	bool addMesh(const Mesh& mesh);
};