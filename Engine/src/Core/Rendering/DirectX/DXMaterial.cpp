#include "pch.h"
#include "DXMaterial.h"
#include "Core/Model/ModelHandler.h"

DXMaterial::DXMaterial() 
	: 
	diffuseMap(nullptr),
	roughnessMap(nullptr),
	metallicMap(nullptr),
	ambientOcclussionMap(nullptr),
	normalMap(nullptr)
{
#ifdef ANK_DX11
	setAlbedoMap(ModelHandler::Get().getTexture(ANK_TEXTURE_DEFAULT_WHITE_PATH));
	setMetallicMap(ModelHandler::Get().getTexture(ANK_TEXTURE_DEFAULT_WHITE_PATH));
	setRoughnessMap(ModelHandler::Get().getTexture(ANK_TEXTURE_DEFAULT_WHITE_PATH));
	setAmbientOcclusionMap(ModelHandler::Get().getTexture(ANK_TEXTURE_DEFAULT_WHITE_PATH));
	setNormalMap(ModelHandler::Get().getTexture(ANK_TEXTURE_DEFAULT_NORMAL_PATH));
#endif
}

DXMaterial::~DXMaterial()
{

}

void DXMaterial::setAlbedoMap(DXTexture* diffuse)
{
	this->diffuseMap = diffuse;
}

const DXTexture& DXMaterial::getAlbedoMap() const
{
	return *this->diffuseMap;
}

void DXMaterial::setRoughnessMap(DXTexture* roughness)
{
	this->roughnessMap = roughness;
}

const DXTexture& DXMaterial::getRoughnessMap() const
{
	return *this->roughnessMap;
}

void DXMaterial::setMetallicMap(DXTexture* metallic)
{
	this->metallicMap = metallic;
}

const DXTexture& DXMaterial::getMetallicMap() const
{
	return *this->metallicMap;
}

void DXMaterial::setAmbientOcclusionMap(DXTexture* ao)
{
	this->ambientOcclussionMap = ao;
}

const DXTexture& DXMaterial::getAmbientOcclusionMap() const
{
	return *this->ambientOcclussionMap;
}

void DXMaterial::setNormalMap(DXTexture* normal)
{
	this->normalMap = normal;
}

const DXTexture& DXMaterial::getNormalMap() const
{
	return *this->normalMap;
}
