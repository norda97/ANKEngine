
#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <array>
#include <exception>
#include <unordered_map>
#include <map>
#include <memory>
#include <queue>
#include <random>

// include the Direct3D Library file
#include <d3d11.h>
#include <d3dcompiler.h>
#include <SimpleMath.h>

#pragma comment (lib, "d3d11.lib")

// ComPtr
#include <wrl/client.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#define ANK_DX11
#define ANK_USE_IMGUI 1
#define ANK_OPENGL 0

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace SimpleMath;

#include "Core/Utils/Logger.h"
using namespace Logger;

#define ANK_TEXTURE_PATH "../Assets/Textures/"
#define ANK_SHADER_PATH "../Assets/Shaders/"
#define ANK_MODEL_PATH "../Assets/Models/"

#define ANK_TEXTURE_DEFAULT_BLACK_PATH "../Assets/Textures/defaultBlack.png"
#define ANK_TEXTURE_DEFAULT_WHITE_PATH "../Assets/Textures/defaultWhite.png"
#define ANK_TEXTURE_DEFAULT_NORMAL_PATH "../Assets/Textures/defaultNormal.png"

#ifdef ANK_DX11
using Vec4 = Vector4;
using Vec3 = Vector3;
using Vec2 = Vector2;
using Mat4 = Matrix;
#else
#include "Vec4.h"
#include "Vec3.h"
#include "Vec2.h"
#endif

