#include "pch.h"
#include "ModelHandler.h"

#include <assimp/postprocess.h>

#ifdef ANK_DX11
	#include "Core/Rendering/DirectX/DXModel.h"
#endif

ModelHandler::~ModelHandler()
{
}

ModelHandler& ModelHandler::get()
{
	static ModelHandler instance;

	return instance;
}

std::shared_ptr<Model> ModelHandler::getModel(const std::string& key)
{
	auto it = this->modelMap.find(key);
	if (it == this->modelMap.end()) {
		ANK_WARNING("Model is not accessible with key: %s", key);
		return std::shared_ptr<Model>();
	}
	else
	{
		return this->modelMap[key];
	}
}

bool ModelHandler::loadModel(const std::string& path, const std::string& key)
{
#ifdef ANK_DX11
	DXModel* model = new DXModel();
#endif

	const aiScene* modelScene = this->importer.ReadFile(path,
		aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_PreTransformVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_Triangulate |
		aiProcess_FixInfacingNormals |
		aiProcess_FindInvalidData |
		aiProcess_ValidateDataStructure | 0);

	if (!modelScene)
		ANK_ERROR("Failed to load model %s\n", path.c_str());
	else
		processScene(modelScene, model);

	this->modelMap[key] = std::make_shared<Model>(model);
}

bool ModelHandler::processScene(const aiScene* modelScene, Model * model)
{
	std::vector<aiNode*> nodes;

	nodes.push_back(modelScene->mRootNode);

	//if (modelScene->mNumMeshes > 0) {
	//	for (int i = 0; i < modelScene->mNumMeshes; i++)
	//	{

	//	}
	//}

	while (true) {
		for (int i = 0; i < nodes.size(); i++) {
			aiNode* modelNode = nodes[i];

			// Extract child meshes
			if (modelNode->mNumMeshes > 0) {
				for (int i = 0; i < modelNode->mNumMeshes; i++)
				{
					nodes.push_back(modelNode->mChildren[i]);
				}
			}
			else
				break;
		}
	};


	for (auto modelNode : nodes) {
		for (int i = 0; i < modelNode->mNumMeshes; i++) {
			processMesh(modelScene->mMeshes[i], model);
		}
	}

	return true;
}

bool ModelHandler::processMesh(const aiMesh* mesh, Model* model)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	size_t size = mesh->mNumVertices;
	if (mesh->HasTextureCoords(0))
		size += size * 2;
	if (mesh->HasNormals())
		size *= 2;
	

	vertices.resize(mesh->mNumVertices);
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		// Add positions
		int index = i * 8;
		vertices[index+0] = mesh->mVertices[i].x;
		vertices[index+1] = mesh->mVertices[i].y;
		vertices[index+2] = mesh->mVertices[i].z;

		// Add normals if they 
		if (mesh->HasNormals()) {
			vertices[index+3] = mesh->mNormals[i].x;
			vertices[index+4] = mesh->mNormals[i].y;
			vertices[index+5] = mesh->mNormals[i].z;
		}
		else
		{
			// To do implement normal generation	
			vertices[index+3] = mesh->mNormals[i].x;
			vertices[index+4] = mesh->mNormals[i].y;
			vertices[index+5] = mesh->mNormals[i].z;
		}

		if (mesh->HasTextureCoords(0)) {
			vertices[index+6] = mesh->mTextureCoords[0][i].x;
			vertices[index+7] = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertices[index+6] = 0;
			vertices[index+7] = 0;
		}
	}

	aiFace* face;
	indices.resize(mesh->mNumFaces * 3);
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		face = &mesh->mFaces[i];
		int index = i * 3;
		indices[index] = (face->mIndices[0]);
		indices[index+1] = (face->mIndices[1]);
		indices[index+2] = (face->mIndices[2]);
	}

	return true;
}
