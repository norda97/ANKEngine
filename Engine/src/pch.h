#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <exception>
#include <unordered_map>
#include <memory>

// include the Direct3D Library file
#include <d3d11.h>
#include <d3dcompiler.h>
//#include "SimpleMath.h"
#include <SimpleMath.h>
#pragma comment (lib, "d3d11.lib")

using namespace DirectX;
using namespace SimpleMath;

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define ANK_DX11
//#define ANK_OPENGL


#ifdef ANK_DEBUG
#include <crtdbg.h>
#define ANK_ERROR(errorMsg) printf(errorMsg)
#define ANK_WARNING(warningMsg) printf(warningMsg)
#define ANK_INFO(warningMsg) printf(warningMsg)
#else

#define ANK_ERROR(errorMsg)
#define ANK_WARNING(warningMsg)
#endif

#define ANK_SHADER_PATH "../Engine/src/Assets/Shaders/"
