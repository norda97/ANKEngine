#pragma once

#include "imgui.h"
#define ANK_DEBUG_INTERFACE 1

class ANKDebugInterface
{
private:
	ANKDebugInterface(){};
	ANKDebugInterface(const ANKDebugInterface& other) = delete;
	ANKDebugInterface(ANKDebugInterface&& other) = delete;
public:
	~ANKDebugInterface() = default;

	static void Display();

	static void Resize(float width, float height);

private:
	static ImVec2 s_Pos;
	static ImVec2 s_Size;
	static ImVec2 s_CoverFactor;
};