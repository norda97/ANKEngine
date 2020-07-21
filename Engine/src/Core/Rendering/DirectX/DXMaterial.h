#pragma once

#include "DXTexture.h"
#include "Core/Model/Material.h"

class Mesh;

class DXMaterial : public Material
{
public:
	DXMaterial();
	~DXMaterial();

	void setAlbedoMap(DXTexture* diffuse);
	const DXTexture& getAlbedoMap() const;

	void setRoughnessMap(DXTexture* roughness);
	const  DXTexture& getRoughnessMap() const;

	void setMetallicMap(DXTexture* metallic);
	const DXTexture& getMetallicMap() const;

	void setAmbientOcclusionMap(DXTexture* ao);
	const DXTexture& getAmbientOcclusionMap() const;

	void setNormalMap(DXTexture* normal);
	const DXTexture& getNormalMap() const;
private:
	DXTexture* diffuseMap;
	DXTexture* roughnessMap;
	DXTexture* metallicMap;
	DXTexture* ambientOcclussionMap;
	DXTexture* normalMap;
};