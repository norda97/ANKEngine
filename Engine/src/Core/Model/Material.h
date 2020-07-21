#pragma once

class DXTexture;
class Entity;
class Model;
class MeshInstance;

struct alignas(16) MaterialProperties
{
	Vector4 albedo;
	float roughness;
	float metallicness;
	float specular;
};

class Material
{
public:
	Material();
	Material(const Material& material) noexcept;
	Material(Material&& material) noexcept;
	virtual ~Material();

	Material& operator= (const Material& material) noexcept;

	const MaterialProperties& getProperties() const;

	void setAlbedo(const Vector4& albedo);
	Vector3 getAlbedo();

	void setRoughness(float roughness);
	float getRoughness();

	void setMetallicness(float metallicness);
	float getMetallicness();

	void setSpecular(float specular);
	float getSpecular();

	virtual void setAlbedoMap(DXTexture * albedo) = 0;
	const virtual DXTexture& getAlbedoMap() const = 0;

	virtual void setRoughnessMap(DXTexture* roughness) = 0;
	const virtual DXTexture& getRoughnessMap() const = 0;

	virtual void setMetallicMap(DXTexture* metallic) = 0;
	const virtual DXTexture& getMetallicMap() const = 0;

	virtual void setAmbientOcclusionMap(DXTexture* ao) = 0;
	const virtual DXTexture& getAmbientOcclusionMap() const = 0;

	virtual void setNormalMap(DXTexture* ao) = 0;
	const virtual DXTexture& getNormalMap() const = 0;

	const std::list<const MeshInstance*>& getRenderList() const;

	void addMeshInstance(const MeshInstance* mesh);
	void removeMeshInstance(const MeshInstance* mesh);

	void addModel(const Model* model);
	void removeModel(const Model* model);

private:
	MaterialProperties matProps;

	std::list<const MeshInstance*> renderList;
};