#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <exception>

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

#ifdef MINI_DEBUG
#include <crtdbg.h>
#endif // !TS_DEBUG


#define MINI_SHADER_PATH "../Engine/src/Assets/Shaders/"