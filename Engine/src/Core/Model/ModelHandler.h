#pragma once

#include "assimp/Importer.hpp"
#include <assimp/scene.h>

class Model;

class ModelHandler
{
public:
	~ModelHandler();

	static ModelHandler& get();

	std::shared_ptr<Model> getModel(const std::string& key);
	bool loadModel(const std::string& path, const std::string& key);
private:
	ModelHandler();
	ModelHandler(const ModelHandler& other) = delete;
	ModelHandler(ModelHandler&& other) = delete;

	bool processScene(const aiScene* modelScene, Model* model);
	bool processMesh(const aiMesh* mesh, Model* model);

	static Assimp::Importer importer;

	static std::unordered_map<std::string, std::shared_ptr<Model>> modelMap;
};