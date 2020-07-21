#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <array>
#include <exception>
#include <unordered_map>
#include <memory>

// include the Direct3D Library file
#include <d3d11.h>
#include <d3dcompiler.h>
//#include "SimpleMath.h"
#include <SimpleMath.h>
#pragma comment (lib, "d3d11.lib")

// ComPtr
#include <wrl/client.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace SimpleMath;

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080
#define WINDOW_SIZE_FACTOR 0.5

#define ANK_DX11
//#define ANK_OPENGL

#define ANK_USE_IMGUI

#ifdef ANK_DEBUG
	#include <crtdbg.h>
	#define ANK_ERROR(...) printf("[ERROR] ");printf(__VA_ARGS__);
	#define ANK_WARNING(...) printf("[WARNING] ");printf(__VA_ARGS__);
	#define ANK_INFO(...) printf("[INFO] ");printf(__VA_ARGS__);
	#define ANK_ASSERT(exp, ...) if(!exp){printf("%s, line %d:\n\t->", __FILE__, __LINE__);printf(__VA_ARGS__);}assert(exp);
	#define _CRT_SECURE_NO_WARNINGS
#else

	#define ANK_ERROR(errorMsg)
	#define ANK_WARNING(warningMsg)
	#define ANK_INFO(warningMsg)
	#define ANK_ASSERT(exp, msg)
#endif

#define ANK_TEXTURE_PATH "../Engine/src/Assets/Textures/"
#define ANK_SHADER_PATH "../Engine/src/Assets/Shaders/"
#define ANK_MODEL_PATH "../Engine/src/Assets/Models/"

#define ANK_TEXTURE_DEFAULT_BLACK_PATH "../Engine/src/Assets/Textures/defaultBlack.png"
#define ANK_TEXTURE_DEFAULT_WHITE_PATH "../Engine/src/Assets/Textures/defaultWhite.png"
#define ANK_TEXTURE_DEFAULT_NORMAL_PATH "../Engine/src/Assets/Textures/defaultNormal.png"

