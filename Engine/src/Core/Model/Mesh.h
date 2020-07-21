#pragma once

class Buffer;

struct VertexData
{
	Vector3 position;
	Vector3 normal;
	Vector3 tangent;
	Vector2 texCoord;
};

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	unsigned getIndexCount() const;
	void setIndexCount(unsigned count);

	void setVertexBuffer(Buffer* buffer);
	void setIndexBuffer(Buffer* buffer);

	const Buffer* getVertexBuffer() const;
	const Buffer* getIndexBuffer() const;
public:
	// Need to be filled out somewhere!
	bool hasIndices;
	bool hasNormals;
	bool hasTangents;
	bool hasTexCoords;

protected:
	Buffer* vertexBuffer;
	Buffer* indexBuffer;

	unsigned indexCount;
};