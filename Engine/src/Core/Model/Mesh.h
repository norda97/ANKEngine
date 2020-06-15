#pragma once

class Mesh
{
public:
	Mesh();
	~Mesh();

	virtual void setVertices(const std::vector<float>& vertices) = 0;
	virtual void setIndices(const std::vector<unsigned int>& indices) = 0;

	bool usesIndices;
	bool usesTexCoords;
	bool usesNormals;

protected:
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

};