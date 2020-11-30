#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "ModelTypes.h"
#include "Core/Model/MeshInstance.h"

#include "Core/Model/Model.h"

class Material;
class Model;
class Mesh;

// Create DXModelHandler
class DXTexture;
class Buffer;

class ModelHandler
{
public:
	~ModelHandler();

	static ModelHandler& get();

	Material* duplicateMaterial(Material* material);
	MaterialID createMaterial(Vector4& albedo, float roughness, float metallicness);

	Model& duplicateModel(const std::string& modelKey, const std::string& newKey);
	Model& duplicateModel(const Model& model, const std::string& newKey);

	Model& getModel(const std::string& name);
	Model& getModel(ModelID id);
	Material* getMaterial(MaterialID materialID);
	Mesh* getMesh(MeshID meshID);

	const std::unordered_map<ModelID, Model>& getModels();
	const std::vector<Material*>& getMaterials();
	DXTexture* getTexture(const std::string& key);

	Model& loadModel(const std::string& path, const std::string& file, const std::string& key);
	
	void shutdown();
private:
	ModelHandler();
	ModelHandler(const ModelHandler& other) = delete;
	ModelHandler(ModelHandler&& other) = delete;

	bool processScene(const std::string& path, const aiScene* modelScene, Model& model);
	bool processMaterial(const std::string& path, const aiMaterial* aiMat, Material* mat);
	bool processMaterialTexture(aiTextureType texType, const std::string& path, const aiMaterial* aiMat, Material* mat);
	bool processMesh(const aiMesh* aiMesh, Mesh* mesh);

	Vector3 calcTangent(const aiFace& face, const aiMesh* aiMesh);

	// Texture management - Move to class
	std::unordered_map<std::string, DXTexture*> textureMap;

	// Vertex buffers
	std::list<Buffer*> bufferMap;

	Assimp::Importer importer;

	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;

	std::unordered_map<ModelID, Model> modelMap;
	std::unordered_map<std::string, ModelID> nameToIndex;
	ModelID modelCount;
};