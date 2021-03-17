#pragma once

#include "imgui.h"

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


private:
	static ImVec2 s_Pos;
	static ImVec2 s_Size;
	static ImVec2 s_CoverFactor;
};