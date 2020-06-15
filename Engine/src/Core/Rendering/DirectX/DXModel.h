#pragma once

#include "DXMesh.h"

#include "Core/Model/Model.h"

class DXModel : public Model

{
public:
	DXModel();
	~DXModel();

	bool addMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
private:
	std::vector<std::shared_ptr<DXMesh>> meshes;
};