#pragma once


class Model
{
public:
	Model();
	~Model();

	virtual bool addMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) = 0;
};