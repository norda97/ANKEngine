#include "pch.h"
#include "ModelHandler.h"

#include <assimp/postprocess.h>

#include "Core/Model/Mesh.h"
#include "Core/Model/MeshInstance.h"
#include "Core/Model/Model.h"
#include "Core/Rendering/General/Buffer.h"

#ifdef ANK_DX11
	#include "Core/Rendering/DirectX/DXMaterial.h"
	#include "Core/Rendering/DirectX/DXBuffer.h"
#endif

ModelHandler::ModelHandler() : modelCount(0)
{
	// Load default diffuse texture
	DXTexture* defaultTexture = new DXTexture();
	defaultTexture->loadTexture(ANK_TEXTURE_DEFAULT_BLACK_PATH);
	this->textureMap[ANK_TEXTURE_DEFAULT_BLACK_PATH] = defaultTexture;

	defaultTexture = new DXTexture();
	defaultTexture->loadTexture(ANK_TEXTURE_DEFAULT_WHITE_PATH);
	this->textureMap[ANK_TEXTURE_DEFAULT_WHITE_PATH] = defaultTexture;

	defaultTexture = new DXTexture();
	defaultTexture->loadTexture(ANK_TEXTURE_DEFAULT_NORMAL_PATH);
	this->textureMap[ANK_TEXTURE_DEFAULT_NORMAL_PATH] = defaultTexture;
}

ModelHandler::~ModelHandler()
{

}

ModelHandler& ModelHandler::get()
{
	static ModelHandler instance;

	return instance;
}

Material* ModelHandler::createMaterial(Material* material)
{
#ifdef ANK_DX11
	DXMaterial* newMaterial = new DXMaterial(*static_cast<DXMaterial*>(material));
	this->materials.push_back(newMaterial);
#elif
	ANK_ASSERT(false, "FAILED due to no other implementation than dx11")
#endif
	return newMaterial;
}

Material* ModelHandler::createMaterial()
{
#ifdef ANK_DX11
	DXMaterial* newMaterial = new DXMaterial();
	this->materials.push_back(newMaterial);
#elif
	ANK_ASSERT(false, "FAILED due to no other implementation than dx11")
#endif
	return newMaterial;
}

//Model* ModelHandler::duplicateModel(const std::string& key, const std::string& newKey)
//{
//	Model* model = getModel(key);
//	if (model) {
//		Model* dupe = new Model(*model);
//
//		this->modelMap[newKey] = dupe;
//		return dupe;
//	}
//
//	return nullptr;
//}
//
//Model* ModelHandler::duplicateModel(Model* model, const std::string& newKey)
//{
//	if (model && this->modelMap.find(newKey) == this->modelMap.end()) {
//		Model* dupe = new Model(*model);
//
//		this->modelMap[newKey] = dupe;
//		return dupe;
//	}
//	else {
//		ANK_WARNING("Failed to duplicate model");
//	}
//	
//	return nullptr;
//}

Model* ModelHandler::getModel(const std::string& name)
{
	auto it = this->nameToIndex.find(name);
	if (it == this->nameToIndex.end()) {
		ANK_WARNING("Model is not accessible with name: %s", name);
		return nullptr;
	}

	return getModel(it->second);
}

Model* ModelHandler::getModel(ModelID id)
{
	auto it = this->modelMap.find(id);
	if (it == this->modelMap.end()) {
		ANK_WARNING("Model is not accessible with modelID: %s", id);
		return nullptr;
	}
	return this->modelMap[id];
}

Material* ModelHandler::getMaterial(MaterialID materialID)
{
	return this->materials[materialID];
}

Mesh* ModelHandler::getMesh(MeshID meshID)
{
	return this->meshes[meshID];
}

const std::unordered_map<ModelID, Model*>& ModelHandler::getModels()
{
	return this->modelMap;
}

const std::vector<Material*>& ModelHandler::getMaterials()
{
	return this->materials;
}

DXTexture* ModelHandler::getTexture(const std::string& key)
{
	return this->textureMap[key];
}

ModelID ModelHandler::loadModel(const std::string& path, const std::string& file, const std::string& name)
{

	if (this->nameToIndex.find(name) != this->nameToIndex.end()) {
		ANK_WARNING("A model has already been loaded with that name!");
		
		// Temporary assert
		ANK_ASSERT(true, "FAILED TO LOAD MODEL");

		// Return default model
		return 0;
	}

	Model* model = new Model();
	
	ModelID modelID = modelCount++;
	this->nameToIndex[name] = modelID;
	this->modelMap[modelID] = model;

	std::string filepath = path + file;
	const aiScene* modelScene = this->importer.ReadFile(filepath,
		aiProcess_MakeLeftHanded | aiProcess_FlipUVs | aiProcess_PreTransformVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_Triangulate |
		aiProcess_FixInfacingNormals |
		aiProcess_FindInvalidData |
		aiProcess_ValidateDataStructure | 0);

	ANK_ASSERT(modelScene, "Failed to load model: %s\n", filepath.c_str());
	ANK_INFO("Loading Model %s\n", filepath.c_str());

	processScene(path, modelScene, model);

	return modelID;
}

void ModelHandler::shutdown()
{
	ANK_WARNING("Shutting down ModelHandler");

	// Clear meshes
	for (Mesh* mesh : this->meshes)
	{
		if (mesh) {
			delete mesh;
			mesh = nullptr;
		}
	}
	this->meshes.clear();

	// Clear meshInstances
	//for (MeshInstance* meshInstance : this->meshInstances)
	//{
	//	if (meshInstance) {
	//		delete meshInstance;
	//		meshInstance = nullptr;
	//	}
	//}
	//this->meshInstances.clear();

	// Clear models
	for (auto pair : this->modelMap)
	{
		if (pair.second) {
			delete pair.second;
			pair.second = nullptr;
		}
	}
	this->modelMap.clear();

	// Clear vertex and index buffers
	for (Buffer* buffer : this->bufferMap)
	{
		delete buffer;
		buffer = nullptr;
	}
	this->bufferMap.clear();

	// Clear textures
	for (auto pair : this->textureMap)
	{
		if (pair.second) {
			delete pair.second;
			pair.second = nullptr;
		}
	}
	this->textureMap.clear();

	// Clear materials
	for (Material* mat : this->materials)
	{
		if (mat) {
			delete mat;
			mat = nullptr;
		}
	}
	this->materials.clear();
}

bool ModelHandler::processScene(const std::string& path, const aiScene* modelScene, Model* model)
{
	std::vector<aiNode*> nodes;

	nodes.push_back(modelScene->mRootNode);

	bool run = true;
	while (run) {
		for (unsigned i = 0; i < nodes.size(); i++) {
			aiNode* modelNode = nodes[i];

			// Extract child meshes
			if (modelNode->mNumChildren > 0) {
				for (unsigned i = 0; i < modelNode->mNumChildren; i++)
				{
					nodes.push_back(modelNode->mChildren[i]);
				}
			}
			else
				run = false;
		}
	};

	// load materials
	unsigned prevTotalMats = this->materials.size();
	if (modelScene->HasMaterials())
	{
		unsigned sceneMatCount = modelScene->mNumMaterials;
		this->materials.resize(this->materials.size() + sceneMatCount);

		for (unsigned i = 0; i < sceneMatCount; i++)
		{
#ifdef ANK_DX11
			Material* mat = new DXMaterial();
#endif
			processMaterial(path, modelScene->mMaterials[i], mat);

			this->materials[prevTotalMats + i] = mat;
		}
		ANK_INFO("Loaded %d materials\n", sceneMatCount);
	}

	// Load mesh
	for (auto modelNode : nodes) {
		for (unsigned i = 0; i < modelNode->mNumMeshes; i++) {
			aiMesh* aiMesh = modelScene->mMeshes[modelNode->mMeshes[i]];

			MaterialID materialIndex = prevTotalMats + aiMesh->mMaterialIndex;
			Material* material = this->materials[materialIndex];

			Mesh* mesh = new Mesh();
			processMesh(aiMesh, mesh);

			MeshInstance meshInstance = { this->meshes.size(), materialIndex };

			this->meshes.push_back(mesh);

			model->addMeshInstance(meshInstance);
		}
	}

	return true;
}

bool ModelHandler::processMaterial(const std::string& path, const aiMaterial* aiMat, Material* mat)
{
	// Diffuse texture
	if (!processMaterialTexture(aiTextureType_DIFFUSE, path, aiMat, mat)) {
		ANK_WARNING("\tFailed to find diffuse texture for model, Opting for default diffuse\n");
	}

	if (!processMaterialTexture(aiTextureType_AMBIENT, path, aiMat, mat)) {
		ANK_WARNING("\tFailed to find metallic texture for model, Opting for default metallic\n");
	}

	if (!processMaterialTexture(aiTextureType_SHININESS, path, aiMat, mat)) {
		ANK_WARNING("\tFailed to find roughness texture for model, Opting for default roughness\n");
	}

	if (!processMaterialTexture(aiTextureType_HEIGHT, path, aiMat, mat)) {
		ANK_WARNING("\tFailed to find normal texture for model, Opting for default normal\n");
	}

	mat->setAmbientOcclusionMap(this->textureMap[ANK_TEXTURE_DEFAULT_WHITE_PATH]);

	//float value = 1.0;
	//aiColor3D colorValue;
	//if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, colorValue) == AI_SUCCESS) {
	//	mat->setAlbedo(Vector4(colorValue.r, colorValue.g, colorValue.b, 1.0f));
	//}

	//if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, colorValue) == AI_SUCCESS) {
	//	mat->setMetallicness(colorValue.r);
	//}

	//if (aiMat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
	//	const float min = 0.025f;
	//	const float max = 1.0f;
	//	mat->setRoughness(min + (max - min) * value);
	//}

	return true;
}

bool ModelHandler::processMaterialTexture(aiTextureType texType, const std::string& path, const aiMaterial* aiMat, Material* mat)
{
	if (aiMat->GetTextureCount(texType) > 0)
	{
		aiString texFileName;
		if (aiMat->GetTexture(texType, 0, &texFileName, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string texPath = std::string(path).append(texFileName.C_Str());
			// Check if texture already is loaded
			if (this->textureMap.find(texPath) == this->textureMap.end())
			{
#ifdef ANK_DX11
				DXTexture* matTex = new DXTexture();
#endif

				if (matTex->loadTexture(texPath))
				{
					switch (texType) {
					case aiTextureType_DIFFUSE:
						mat->setAlbedoMap(matTex);
						break;
					case aiTextureType_AMBIENT: // Metallic
						mat->setMetallicMap(matTex);
						break;
					case aiTextureType_SHININESS: // Roughness
						mat->setRoughnessMap(matTex);
						break;
					case aiTextureType_HEIGHT: // Roughness
						mat->setNormalMap(matTex);
						break;
					}

					this->textureMap[texPath] = matTex;
					return true;
				}
				else
					delete matTex;
			}
		}
	}

//#ifdef ANK_DX11
//	switch (texType) {
//	case aiTextureType_DIFFUSE:
//		mat->setAlbedoMap(this->textureMap[ANK_TEXTURE_DEFAULT_WHITE_PATH]);
//		break;
//	case aiTextureType_AMBIENT: // Metallic
//		mat->setMetallicMap(this->textureMap[ANK_TEXTURE_DEFAULT_WHITE_PATH]);
//		break;
//	case aiTextureType_SHININESS: // Roughness
//		mat->setRoughnessMap(this->textureMap[ANK_TEXTURE_DEFAULT_WHITE_PATH]);
//		break;
//	case aiTextureType_HEIGHT: // Roughness
//		mat->setNormalMap(this->textureMap[ANK_TEXTURE_DEFAULT_NORMAL_PATH]);
//		break;
//	}
//#endif

	return false;
}

bool ModelHandler::processMesh(const aiMesh* aiMesh, Mesh* mesh)
{
	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;

	vertices.resize(aiMesh->mNumVertices);

	const bool hasNormals = aiMesh->HasNormals();
	const bool hasTangents = aiMesh->HasTangentsAndBitangents();

	for (unsigned i = 0; i < aiMesh->mNumVertices; i++)
	{
		// Add positions
		int index = i * 8;
		vertices[i].position.x = aiMesh->mVertices[i].x;
		vertices[i].position.y = aiMesh->mVertices[i].y;
		vertices[i].position.z = aiMesh->mVertices[i].z;

		// Add normals if they exist, which they do due to assimp generating them
		if (hasNormals) {
			vertices[i].normal.x = aiMesh->mNormals[i].x;
			vertices[i].normal.y = aiMesh->mNormals[i].y;
			vertices[i].normal.z = aiMesh->mNormals[i].z;
		}

		// Add tangents if they exist
		if (hasTangents) {
			vertices[i].tangent.x = aiMesh->mTangents[i].x;
			vertices[i].tangent.y = aiMesh->mTangents[i].y;
			vertices[i].tangent.z = aiMesh->mTangents[i].z;
		}

		if (aiMesh->HasTextureCoords(0)) {
			vertices[i].texCoord.x = aiMesh->mTextureCoords[0][i].x;
			vertices[i].texCoord.y = aiMesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertices[i].texCoord.x = 0;
			vertices[i].texCoord.y = 0;
		}
	}

	aiFace* face;
	indices.resize(aiMesh->mNumFaces * 3U);
	for (unsigned i = 0; i < aiMesh->mNumFaces; i++)
	{
		face = &aiMesh->mFaces[i];
		int index = i * 3;
		indices[index] = (face->mIndices[0]);
		indices[index+1] = (face->mIndices[1]);
		indices[index+2] = (face->mIndices[2]);

		if (!hasTangents) {
			Vector3 tangent = calcTangent(*face, aiMesh);
			vertices[face->mIndices[0]].tangent = tangent;
			vertices[face->mIndices[1]].tangent = tangent;
			vertices[face->mIndices[2]].tangent = tangent;
		}
	}

	ANK_INFO("Added mesh\t [Vertices: %d\t Indices: %d]\n", static_cast<int>(vertices.size()), (int)indices.size());

	DXBuffer* vertexBuffer = new DXBuffer();
	vertexBuffer->init(vertices.data(), vertices.size() * sizeof(VertexData), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);

	DXBuffer* indexBuffer = new DXBuffer();
	indexBuffer->init(indices.data(), indices.size() * sizeof(unsigned int), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0);

	mesh->setVertexBuffer(static_cast<Buffer*>(vertexBuffer));
	mesh->setIndexBuffer(static_cast<Buffer*>(indexBuffer));
	mesh->setIndexCount(indices.size());

	this->bufferMap.push_back(vertexBuffer);
	this->bufferMap.push_back(indexBuffer);

	return true;
}


Vector3 ModelHandler::calcTangent(const aiFace& face, const aiMesh* aiMesh)
{
	Vector3 V1 = { aiMesh->mVertices[face.mIndices[0]].x, aiMesh->mVertices[face.mIndices[0]].y, aiMesh->mVertices[face.mIndices[0]].z };
	Vector3 V2 = { aiMesh->mVertices[face.mIndices[1]].x, aiMesh->mVertices[face.mIndices[1]].y, aiMesh->mVertices[face.mIndices[1]].z };
	Vector3 V3 = { aiMesh->mVertices[face.mIndices[2]].x, aiMesh->mVertices[face.mIndices[2]].y, aiMesh->mVertices[face.mIndices[2]].z };

	Vector2 UV1 = { aiMesh->mTextureCoords[0][face.mIndices[0]].x, aiMesh->mTextureCoords[0][face.mIndices[0]].y };
	Vector2 UV2 = { aiMesh->mTextureCoords[0][face.mIndices[1]].x, aiMesh->mTextureCoords[0][face.mIndices[1]].y };
	Vector2 UV3 = { aiMesh->mTextureCoords[0][face.mIndices[2]].x, aiMesh->mTextureCoords[0][face.mIndices[2]].y };

	Vector3 E1 = V2 - V1;
	Vector3 E2 = V3 - V1;
	Vector2 deltaUV1 = UV3 - UV1;
	Vector2 deltaUV2 = UV3 - UV1;

	Vector3 Tangent;
	
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

	Tangent.x = f * (deltaUV2.y * E1.x - deltaUV1.y * E2.x);
	Tangent.y = f * (deltaUV2.y * E1.y - deltaUV1.y * E2.y);
	Tangent.z = f * (deltaUV2.y * E1.z - deltaUV1.y * E2.z);

	return Tangent;
}
