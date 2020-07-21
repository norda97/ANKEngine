#include "pch.h"
#include "DXRenderer.h"

#include "Core/Model/ModelHandler.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"
#include "Core/Rendering/DirectX/DXShader.h"
#include "Core/Rendering/DirectX/DXBuffer.h"
#include "Core/Rendering/DirectX/DXMaterial.h"
#include "Core/Rendering/DirectX/DXTexture.h"

#include "Core/Model/Model.h"
#include "Core/Model/Mesh.h"
#include "Core/Model/MeshInstance.h"

#include "Core/Logic/Scenes/Scene.h"
#include "Core/Logic/Entity.h"
#include "Core/Utils/Cameras/Camera.h"

#include "stbi/stb_image.h"

#ifdef ANK_USE_IMGUI
	#include "imgui.h"
	#include "examples/imgui_impl_win32.h"
	#include "examples/imgui_impl_dx11.h"
#endif

DXRenderer::DXRenderer() : camera(nullptr), instanceThreshold(100)
{

}

DXRenderer::~DXRenderer()
{
#ifdef ANK_USE_IMGUI
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

bool DXRenderer::init()
{
	if (!initStates())
		return false;
	if (!initShaders())
		return false;
	if (!initBuffers())
		return false;

	// temp
	this->sampler.init();

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	float* data = stbi_loadf(std::string(ANK_TEXTURE_PATH).append("Arches_E_PineTree/Arches_E_PineTree_3k.hdr").c_str(), &width, &height, &nrComponents, 0);
	stbi_set_flip_vertically_on_load(false);
	if (data)
	{
		size_t size = width * height * 4;
		float* rgba32 = new float[size];
		ZeroMemory(rgba32, sizeof(float) * width * height * 4);
		unsigned offset = 0;
		for (unsigned i = 0; i < width * height; i++)
		{
			memcpy(rgba32 + offset, data + (i * 3), sizeof(float) * 3);
			offset += 4;
		}

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.CPUAccessFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA srd = { 0 };
		srd.pSysMem = (void*)rgba32;
		srd.SysMemPitch = width * 16;
		srd.SysMemSlicePitch = 0;

		this->equiTexture.init(&srd, texDesc);

		stbi_image_free(data);
		delete[] rgba32;
	}

	renderBRDFLutTex();

	environmentMap.init(512, 512, 1);
	irradianceMap.init(32, 32, 1);
	radianceMap.init(128, 128, 5);

	setupImgui();
	createCubemap(environmentMap, this->equirectangularShader, this->equiTexture.getShaderResource());
	createCubemap(irradianceMap, this->irradianceShader, environmentMap.getResourceView());
	createCubemapMip(radianceMap, this->radianceShader, environmentMap.getResourceView());

	return true;
}

void DXRenderer::render()
{
	
#ifdef ANK_USE_IMGUI
	drawImgui();
#endif

	auto& devcon = DXDeviceInstance::get().getDevCon();
	auto& dev = DXDeviceInstance::get().getDev();

	// Update scene constant buffers
	updateSceneConstants(0.0f);

	// Set linear sampler
	devcon->PSSetSamplers(0, 1, this->sampler.getSampler().GetAddressOf());

	// Clear depth-stencil and gBuffers
	devcon->ClearDepthStencilView(DXDeviceInstance::get().getDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	deferredRenderer.clearRenderTargets();

	auto materials = ModelHandler::get().getMaterials();
	unsigned matCount = materials.size();
	if (matCount > 0)
	{
		// Set constant buffers
		ID3D11Buffer* cBuffers[1] = { this->sceneBuffer.getBuffer().Get() };
		devcon->VSSetConstantBuffers(0, 1, cBuffers);
		devcon->PSSetConstantBuffers(0, 1, this->materialProperties.getBuffer().GetAddressOf());

		// Set gbuffers to rendertargets
		devcon->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
		deferredRenderer.bindRenderTargets(DXDeviceInstance::get().getDepthStencilView().Get());

		devcon->RSSetState(this->rsBackCull.Get());

		// Bind instance world position buffer
		unsigned int strides[1] = { sizeof(Matrix) };
		unsigned int offsets[1] = { 0 };
		ID3D11Buffer* bufferPointers[1] = { this->instanceBuffer.getBuffer().Get() };

		devcon->IASetVertexBuffers(1, 1, bufferPointers, strides, offsets);
		deferredRenderer.bindShaders();


		// For each material, render all instances of models whichs use the material.
		unsigned instanceOffset = 0;
		for (unsigned i = 0; i < matCount; i++)
		{
			if (instanceOffset >= instanceThreshold) {
				ANK_WARNING("Instance buffer not large enough for scheduled work!\n");
				break;
			}

			const Material* mat = materials[i];

			this->materialProperties.update((void*)&mat->getProperties(), sizeof(MaterialProperties), 0, D3D11_MAP_WRITE_DISCARD);

#define TEX_COUNT 5
			ID3D11ShaderResourceView* textures[TEX_COUNT] = {
				mat->getAlbedoMap().getShaderResource().Get(),
				mat->getMetallicMap().getShaderResource().Get(),
				mat->getRoughnessMap().getShaderResource().Get(),
				mat->getAmbientOcclusionMap().getShaderResource().Get(),
				mat->getNormalMap().getShaderResource().Get()
			};

			devcon->PSSetShaderResources(0, TEX_COUNT, textures);

			for (const MeshInstance* meshInstance : mat->getRenderList())
			{
				const std::list<const Entity*>& instances = meshInstance->getInstanceList();
				unsigned instanceCount = instances.size();

				deferredRenderer.renderMeshInstanced(meshInstance->getMesh(), instanceCount, instanceOffset);
				instanceOffset += instanceCount;
			}
		}
	}
	
	devcon->OMSetDepthStencilState(this->noDepthStencilState.Get(), 0);
	devcon->RSSetState(this->rsBackCull.Get());

	ID3D11Buffer* cBuffers[2] = { this->scenePBRBuffer.getBuffer().Get(), this->lightBuffer.getBuffer().Get() };
	devcon->PSSetConstantBuffers(0, 2, cBuffers);
	// Set irrandiance map
	devcon->PSSetShaderResources(4, 1, this->irradianceMap.getResourceView().GetAddressOf());
	devcon->PSSetShaderResources(5, 1, this->radianceMap.getResourceView().GetAddressOf());
	devcon->PSSetShaderResources(6, 1, this->BRDFLutTexture.getShaderResource().GetAddressOf());

	this->deferredRenderer.renderComplete(DXDeviceInstance::get().getBackbuffer().GetAddressOf());

	// Render skybox last to cull fragments and avoid shading
	// Update camera for skybox
	SceneVariables sv = { this->camera->getRotation() * this->camera->getProjection() };
	this->sceneBuffer.update(static_cast<void*>(&sv), sizeof(sv), 0);

	devcon->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	devcon->OMSetRenderTargets(1, DXDeviceInstance::get().getBackbuffer().GetAddressOf(), DXDeviceInstance::get().getDepthStencilView().Get());
	devcon->RSSetState(this->rsFrontCull.Get());

	renderEnvironmentMap(this->skyboxShader, this->environmentMap.getResourceView());

#ifdef ANK_USE_IMGUI
	renderImgui();
#endif

	DXDeviceInstance::get().getSwapchain()->Present(0, 0);
}

void DXRenderer::update(float dt)
{
	static bool update = true;
	updateInstanceConstants();
}

bool DXRenderer::initStates()
{
	auto& device = DXDeviceInstance::get().getDev();

	// Render state
	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(D3D11_RASTERIZER_DESC));
	// Solid renderstate
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_BACK;
	ANK_ASSERT(SUCCEEDED(device->CreateRasterizerState(&rDesc, rsBackCull.GetAddressOf())), "Failed to create render state\n");

	// Solid renderstate
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_FRONT;
	ANK_ASSERT(SUCCEEDED(device->CreateRasterizerState(&rDesc, rsFrontCull.GetAddressOf())), "Failed to create render state\n");

	// Wireframe
	rDesc.FillMode = D3D11_FILL_WIREFRAME;
	rDesc.CullMode = D3D11_CULL_NONE;
	ANK_ASSERT(SUCCEEDED(device->CreateRasterizerState(&rDesc, rsWireframe.GetAddressOf())), "Failed to create wireframe render state\n");

	// Blend state
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.AlphaToCoverageEnable = FALSE;
	blendStateDesc.IndependentBlendEnable = FALSE;
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ANK_ASSERT(SUCCEEDED(device->CreateBlendState(&blendStateDesc, &this->blendState)), "Failed To Create Blend State\n");

	auto& devcon = DXDeviceInstance::get().getDevCon();
	devcon->OMSetBlendState(this->blendState.Get(), NULL, 0xFFFFFF);

	// Depth-stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = false;

	ANK_ASSERT(SUCCEEDED(device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilState.GetAddressOf())), "Failed to create depth stencil state");

	depthStencilDesc = { 0 };
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.StencilEnable = false;

	ANK_ASSERT(SUCCEEDED(device->CreateDepthStencilState(&depthStencilDesc, this->noDepthStencilState.GetAddressOf())), "Failed to create depth stencil state");
	return true;
}

bool DXRenderer::initShaders()
{
	// Create input layout for default shader
	//std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	//{
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(Vector3) * 1, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(Vector3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0}
	//};
	//if(!this->defaultShader.init("DefaultShader_V.hlsl", "DefaultShader_P.hlsl", ied))
	//	return false;

	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	if (!this->equirectangularShader.init("UtilShaders/WorldPosition_V.hlsl", "UtilShaders/EquirectangularSampler_P.hlsl", ied))
		return false;
	
	if (!this->irradianceShader.init("UtilShaders/WorldPosition_V.hlsl", "UtilShaders/CubemapConvolution_P.hlsl", ied))
		return false;

	if (!this->radianceShader.init("UtilShaders/WorldPosition_V.hlsl", "UtilShaders/RadiancePreFilter_P.hlsl", ied))
		return false;

	if (!this->skyboxShader.init("UtilShaders/WorldPosition_V.hlsl", "UtilShaders/Skybox_P.hlsl", ied))
		return false;
	ied =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	if (!this->BRDFLutShader.init("UtilShaders/FullscreenQuad_V.hlsl", "UtilShaders/PreComputeBRDF_P.hlsl", ied))
		return false;

	return true;
}

bool DXRenderer::initBuffers()
{
	// Init constant buffer with scene variables
	if (!this->sceneBuffer.init(NULL, sizeof(SceneVariables), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE))
		return false;

	if (!this->scenePBRBuffer.init(NULL, sizeof(Vector4), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE))
		return false;

	if (!this->instanceBuffer.init(NULL, sizeof(Instance)* instanceThreshold, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE))
		return false;

	if (!this->materialProperties.init(NULL, sizeof(MaterialProperties), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE))
		return false;

	Light light;
	light.position = Vector3(0.f, 0.f, -20.f);
	light.intensity = 1000.0f;

	if (!this->lightBuffer.init(&light, sizeof(Light), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0))
		return false;

	return true;
}

void DXRenderer::setupImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(DXDeviceInstance::get().getHWND());
	ImGui_ImplDX11_Init(DXDeviceInstance::get().getDev().Get(), DXDeviceInstance::get().getDevCon().Get());
}

void DXRenderer::drawImgui()
{
	// Create new frame for imgui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool show = true;
	if (show)
	{

		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		{
			ImGui::Begin("BRDF LUT textures");
			ImGui::Image((void*)BRDFLutTexture.getShaderResource().Get(), ImVec2(256, 256));
			ImGui::End();
		}

	/*	{
			ImGui::Begin("Convulution");
			ImGui::BeginGroup();

			auto& resViews = this->radianceMap.getResourceViews();
			for (unsigned i = 0; i < 6; i++)
			{
				ImGui::Image((void*)resViews[i].Get(), ImVec2(256, 256));
			}

			ImGui::EndGroup();
			ImGui::End();
		}*/

	
	}
}

void DXRenderer::renderImgui()
{
	auto& devcon = DXDeviceInstance::get().getDevCon();

	ImGui::Render();
	devcon->OMSetBlendState(this->blendState.Get(), NULL, 0xFFFFFF);
	devcon->OMSetRenderTargets(1, DXDeviceInstance::get().getBackbuffer().GetAddressOf(), NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DXRenderer::updateInstanceConstants()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };

	DXDeviceInstance::get().getDevCon()->Map(this->instanceBuffer.getBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	auto materials = ModelHandler::get().getMaterials();
	unsigned matCount = materials.size();
	if (matCount > 0)
	{
		// For each material, render all instances of models whichs use the material.
		unsigned instanceIndex = 0;
		for (unsigned i = 0; i < matCount; i++)
		{
			const Material* mat = materials[i];
			for (const MeshInstance* mesh : mat->getRenderList())
			{
				//EXPENSIVE! - Can be solved with single memcpy per entity list
				const std::list<const Entity*>& instances = mesh->getInstanceList();
				for (const Entity* entity : instances)
				{
					memcpy((char*)mappedResource.pData + (instanceIndex++ * sizeof(Instance)), (void*)&entity->getTransform(), sizeof(Instance));
				}
			}
		}
	}

	DXDeviceInstance::get().getDevCon()->Unmap(this->instanceBuffer.getBuffer().Get(), 0);
}

void DXRenderer::setCamera(Camera* camera)
{
	this->camera = camera;
}

void DXRenderer::updateSceneConstants(float dt)
{
	static float elapTime = 0.0;
	elapTime += dt;

	// Update camera for PreRendering
	SceneVariables sv = { this->camera->getViewProjection()};
	this->sceneBuffer.update(static_cast<void*>(&sv), sizeof(sv), 0);

	// Update campos for deferred
	Vector3 camPos = this->camera->getPosition();
	this->scenePBRBuffer.update(&camPos, sizeof(Vector3), 0);
}

void DXRenderer::renderEnvironmentMap(DXShader& shader, const ComPtr<ID3D11ShaderResourceView>& envMap)
{
	auto& devcon = DXDeviceInstance::get().getDevCon();

	// Render equiMap
	Model* cube = ModelHandler::get().getModel("cube");

	shader.prepare();

	const std::vector<MeshInstance*>& meshes = cube->getMeshInstances();

	const Mesh& mesh = meshes[0]->getMesh();

	devcon->PSSetShaderResources(0, 1, envMap.GetAddressOf());

	unsigned int strides[1] = { sizeof(VertexData) };
	unsigned int offsets[1] = { 0 };
	ID3D11Buffer* bufferPointers[1] = { static_cast<const DXBuffer*>(mesh.getVertexBuffer())->getBuffer().Get() };

	devcon->IASetVertexBuffers(0, 1, bufferPointers, strides, offsets);
	devcon->IASetIndexBuffer(static_cast<const DXBuffer*>(mesh.getIndexBuffer())->getBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//DXDeviceInstance::get().getDevCon()->RSSetState(rsWireframe);
	devcon->DrawIndexed(mesh.getIndexCount(), 0, 0);

}

void DXRenderer::createCubemap(DXCubemap& cubemap, DXShader& shader, const ComPtr<ID3D11ShaderResourceView>& envMap)
{
	auto& devcon = DXDeviceInstance::get().getDevCon();
	auto& dev = DXDeviceInstance::get().getDev();

	devcon->PSSetSamplers(0, 1, this->sampler.getSampler().GetAddressOf());

	Matrix proj = Matrix::CreatePerspectiveFieldOfView(XM_PI * 0.5f, 1.0f, .1f, 10.f);

	Matrix mat[6];
	mat[0] =  Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(1.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0));
	mat[1] =  Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(-1.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0));
	mat[2] =  Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0), Vector3(0.0, 0.0, -1.0));
	mat[3] =  Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(0.0, -1.0, 0.0), Vector3(0.0, 0.0, 1.0));
	mat[4] =  Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0), Vector3(0.0, 1.0, 0.0));
	mat[5] =  Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, -1.0), Vector3(0.0, 1.0, 0.0));

	// Set Sampler
	devcon->PSSetSamplers(0, 1, this->sampler.getSampler().GetAddressOf());

	ID3D11Buffer* cBuffers[1] = { this->sceneBuffer.getBuffer().Get() };
	devcon->VSSetConstantBuffers(0, 1, cBuffers);
	devcon->RSSetState(this->rsFrontCull.Get());
	devcon->OMSetDepthStencilState(this->noDepthStencilState.Get(), 0);

	auto& RTs = cubemap.getRenderTargets();
	DXDeviceInstance::get().setViewport(0, 0, cubemap.width, cubemap.height);
	for (unsigned i = 0; i < 6; i++)
	{
		devcon->OMSetRenderTargets(1, RTs[i][0].GetAddressOf(), NULL);

		// Update camera for PreRendering
		SceneVariables sv = {  mat[i] * proj };
		this->sceneBuffer.update(static_cast<void*>(&sv), sizeof(sv), 0);

		// Render skybox last to cull fragments
		renderEnvironmentMap(shader, envMap);
	}
	DXDeviceInstance::get().setViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void DXRenderer::createCubemapMip(DXCubemap& cubemap, DXShader& shader, const ComPtr<ID3D11ShaderResourceView>& envMap)
{
	auto& devcon = DXDeviceInstance::get().getDevCon();
	auto& dev = DXDeviceInstance::get().getDev();

	DXBuffer roughnessBuffer;
	if (!roughnessBuffer.init(NULL, sizeof(float), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE))
		return;

	devcon->PSSetSamplers(0, 1, this->sampler.getSampler().GetAddressOf());

	Matrix proj = Matrix::CreatePerspectiveFieldOfView(XM_PI * 0.5f, 1.0f, .1f, 10.f);

	Matrix mat[6];
	mat[0] = Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(1.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0));
	mat[1] = Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(-1.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0));
	mat[2] = Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0), Vector3(0.0, 0.0, -1.0));
	mat[3] = Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(0.0, -1.0, 0.0), Vector3(0.0, 0.0, 1.0));
	mat[4] = Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0), Vector3(0.0, 1.0, 0.0));
	mat[5] = Matrix::CreateLookAt(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, -1.0), Vector3(0.0, 1.0, 0.0));

	// Set Sampler
	devcon->PSSetSamplers(0, 1, this->sampler.getSampler().GetAddressOf());

	devcon->VSSetConstantBuffers(0, 1, this->sceneBuffer.getBuffer().GetAddressOf());
	devcon->PSSetConstantBuffers(0, 1, roughnessBuffer.getBuffer().GetAddressOf());
	devcon->RSSetState(this->rsFrontCull.Get());
	devcon->OMSetDepthStencilState(this->noDepthStencilState.Get(), 0);

	unsigned int maxMipLevels = cubemap.getMipLevels();
	auto& RTs = cubemap.getRenderTargets();

	for (unsigned mip = 0; mip < maxMipLevels; mip++)
	{
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);

		DXDeviceInstance::get().setViewport(0, 0, mipWidth, mipHeight);
		for (unsigned i = 0; i < 6; i++)
		{
			devcon->OMSetRenderTargets(1, RTs[i][mip].GetAddressOf(), NULL);

			// Update camera for PreRendering
			SceneVariables sv = { mat[i] * proj };
			this->sceneBuffer.update(static_cast<void*>(&sv), sizeof(sv), 0);
			float roughness = (float)mip / (float)(maxMipLevels - 1);
			roughnessBuffer.update((void*)&roughness, sizeof(float), 0);

			// Render skybox last to cull fragments
			renderEnvironmentMap(shader, envMap);
		}
	}

	DXDeviceInstance::get().setViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void DXRenderer::renderBRDFLutTex()
{
	DXBuffer fullscreenTri;

	float vertices[]
	{
		// Pos				// Uv
		-1.f, -1.f, 0.f,	0.f, 0.f,
		-1.f, 3.f, 0.f,		0.f, -2.f,
		3.f, -1.f, 0.f,		2.f, 0.f
	};

	fullscreenTri.init(&vertices, sizeof(float) * 18, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = 512;
	texDesc.Height = 512;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	//D3D11_SUBRESOURCE_DATA srd = { 0 };
	//srd.pSysMem = (void*)image;
	//srd.SysMemPitch = 512 * 8;
	//srd.SysMemSlicePitch = 0;

	this->BRDFLutTexture.init(NULL, texDesc);

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	//.MipSlice = D3D11CalcSubresource(0, 6, tdesc.MipLevels);
	ANK_ASSERT(SUCCEEDED(DXDeviceInstance::get().getDev()->CreateRenderTargetView(this->BRDFLutTexture.getTexture().Get(), &rtvDesc, rtv.ReleaseAndGetAddressOf())),
		"Failed to create render target view for BRDF LUT");

	auto& devcon = DXDeviceInstance::get().getDevCon();
	DXDeviceInstance::get().setViewport(0, 0, 512, 512);

	// Render fullscreen tri to backbuffer
	this->BRDFLutShader.prepare();
	devcon->RSSetState(this->rsBackCull.Get());
	devcon->OMSetDepthStencilState(this->noDepthStencilState.Get(), 0);
	FLOAT clearColor[2] = { 1.f, 0.f };
	devcon->ClearRenderTargetView(rtv.Get(), clearColor);
	devcon->OMSetRenderTargets(1, rtv.GetAddressOf(), NULL);

	unsigned int strides[1] = { sizeof(float) * 5 };
	unsigned int offsets[1] = { 0 };

	devcon->IASetVertexBuffers(0, 1, fullscreenTri.getBuffer().GetAddressOf(), strides, offsets);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	devcon->Draw(3, 0);

	DXDeviceInstance::get().setViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}