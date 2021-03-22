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

struct alignas(16) SSAOConstants
{
	Matrix Projection;
	unsigned screenWidth;
	unsigned screenHeight;
	unsigned padding0;
	unsigned padding1;
	Vector4 samples[64];
};


class DXRenderer
{
public:
	DXRenderer();
	~DXRenderer();

	bool Init();

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

	// SSAO pass
	void GenerateSSAOUtils(unsigned sampleCount);
	void RenderSSAO();

	// Environment pass
	void renderEnvironmentMap(DXShader& shader, const ComPtr<ID3D11ShaderResourceView>& envMap);
	void createCubemap(DXCubemap& m_Cubemap, DXShader& shader, const ComPtr<ID3D11ShaderResourceView>& envMap);
	void createCubemapMip(DXCubemap& m_Cubemap, DXShader& shader, const ComPtr<ID3D11ShaderResourceView>& envMap);
	void renderBRDFLutTex();
	
	const unsigned m_MaxPointLights;
	unsigned m_PointLightCount;

	unsigned m_DisplayRTIndex;

	Camera* camera;

	// Deferred rendering
	DXDeferred m_DeferredRenderer;
	
	// SSAO Shaders
	DXShader m_SSAOShader;

	// SSAO Textures
	DXTexture							m_SSAOTexture;
	ComPtr<ID3D11RenderTargetView>		m_pSSAORenderTarget = nullptr;

	DXTexture m_SSAONoiseTexture;

	// PBR & Environment Shaders
	DXShader equirectangularShader;
	DXShader irradianceShader;
	DXShader radianceShader;
	DXShader skyboxShader;
	DXShader BRDFLutShader;

	// PBR & Environment Texture/Cubemaps
	DXTexture equiTexture;
	DXTexture BRDFLutTexture;
	DXCubemap environmentMap;
	DXCubemap irradianceMap;
	DXCubemap radianceMap;

	DXBuffer materialProperties;

	DXBuffer lightBuffer;
	DXBuffer sceneBuffer;
	DXBuffer scenePBRBuffer;
	DXBuffer m_SSAOBuffer;

	DXBuffer m_FullscreenTri;

	//DXBuffer instanceBuffer;

	DXSampler m_SamplerLinearWrap;
	DXSampler m_SamplerPointClamp;
	DXSampler m_SamplerPointRepeat;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rsFrontCull				= nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rsBackCull				= nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rsWireframe				= nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState		= nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		noDepthStencilState		= nullptr;
	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState				= nullptr;
};