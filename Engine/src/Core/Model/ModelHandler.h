#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Material;
class Model;
class Mesh;
class MeshInstance;

// Create DXModelHandler
class DXTexture;
class Buffer;


class ModelHandler
{
public:
	~ModelHandler();

	static ModelHandler& get();

	Material* createMaterial(Material* material);
	Material* createMaterial();

	Model* duplicateModel(const std::string& modelKey, const std::string& newKey);
	Model* duplicateModel(Model* model, const std::string& newKey);

	Model* getModel(const std::string& key);
	const std::unordered_map<std::string, Model*>& getModels();
	const std::vector<Material*>& getMaterials();
	DXTexture* getTexture(const std::string& key);

	Model* loadModel(const std::string& path, const std::string& file, const std::string& key);
	
	void shutdown();
private:
	ModelHandler();
	ModelHandler(const ModelHandler& other) = delete;
	ModelHandler(ModelHandler&& other) = delete;

	bool processScene(const std::string& path, const aiScene* modelScene, Model* model);
	bool processMaterial(const std::string& path, const aiMaterial* aiMat, Material* mat);
	bool processMaterialTexture(aiTextureType texType, const std::string& path, const aiMaterial* aiMat, Material* mat);
	bool processMesh(const aiMesh* aiMesh, Mesh* mesh);

	Vector3 calcTangent(const aiFace& face, const aiMesh* aiMesh);

	// Texture management - Move to class
	std::unordered_map<std::string, DXTexture*> textureMap;

	std::list<Buffer*> bufferMap;

	Assimp::Importer importer;

	std::vector<Material*> materials;
	std::list<Mesh*> meshes;
	//std::list<MeshInstance*> meshInstances;
	std::unordered_map<std::string, Model*> modelMap;
};