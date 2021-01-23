#pragma once

#include "Core/Rendering/DirectX/DXShader.h"
#include "Core/Rendering/DirectX/DXTexture.h"
#include "Core/Rendering/DirectX/DXSampler.h"
#include "Core/Rendering/DirectX/DXDeferred.h"
#include "Core/Rendering/DirectX/DXCubemap.h"

#include "Core/Model/ModelTypes.h"

class Camera;

struct Instance
{
	Matrix world;
};

struct Light
{
	Vector3 position;
	float intensity;
};

class DXRenderer
{
public:
	DXRenderer();
	~DXRenderer();

	bool init();

	void ResizeGBuffers(float width, float height);

	void setCamera(Camera* camera);
	void prepare();
	void setMaterial(MaterialID materialID);

	void finishFrame();

private:
	bool initStates();
	bool InitShaders();
	bool initBuffers();

	void setupImgui();
	void drawImgui();
	void renderImgui();

	void updateSceneConstants(float dt);

	void renderEnvironmentMap(DXShader& shader, const ComPtr<ID3D11ShaderResourceView>& envMap);
	void createCubemap(DXCubemap& m_Cubemap, DXShader& shader, const ComPtr<ID3D11ShaderResourceView>& envMap);
	void createCubemapMip(DXCubemap& m_Cubemap, DXShader& shader, const ComPtr<ID3D11ShaderResourceView>& envMap);
	void renderBRDFLutTex();
	//void renderModel(DXModel& model, unsigned instanceCount, unsigned instanceOffset);
	
	const unsigned maxPointLights;
	unsigned pointLightCount;

	Camera* camera;

	// Deferred rendering
	DXDeferred m_DeferredRenderer;
	
	// Other
	DXShader equirectangularShader;
	DXShader irradianceShader;
	DXShader radianceShader;
	DXShader skyboxShader;
	DXShader BRDFLutShader;

	DXTexture equiTexture;
	DXTexture BRDFLutTexture;
	DXCubemap environmentMap;
	DXCubemap irradianceMap;
	DXCubemap radianceMap;

	DXBuffer materialProperties;

	DXBuffer lightBuffer;
	DXBuffer sceneBuffer;
	DXBuffer scenePBRBuffer;
	//DXBuffer instanceBuffer;

	DXSampler samplerLinear;
	DXSampler samplerPoint;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rsFrontCull				= nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rsBackCull				= nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rsWireframe				= nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState		= nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		noDepthStencilState		= nullptr;
	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState				= nullptr;
};