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

	void Display();

	static void Resize(float width, float height);

	static ANKDebugInterface& Get()
	{
		if (s_pInstance == nullptr)
			s_pInstance = new ANKDebugInterface();

		return *s_pInstance;
	}

	void Release()
	{
		delete s_pInstance;
	}

private:
	static ImVec2 s_Pos;
	static ImVec2 s_Size;
	static ImVec2 s_CoverFactor;

	static ANKDebugInterface* s_pInstance;
};