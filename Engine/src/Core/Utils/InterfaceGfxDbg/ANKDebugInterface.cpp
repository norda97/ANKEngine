#include "pch.h"
#include "ANKDebugInterface.h"

ImVec2 ANKDebugInterface::s_Pos = ImVec2(0.0f, 0.0f);
ImVec2 ANKDebugInterface::s_Size = ImVec2(100, 100);
ImVec2 ANKDebugInterface::s_CoverFactor = ImVec2(0.25f, 1.0f);

std::vector<std::function<void()>> ANKDebugInterface::s_InterfaceCallbacks;

void ANKDebugInterface::Display()
{
	// SidePanel
	ImGui::SetNextWindowPos(s_Pos);
	ImGui::SetNextWindowSize(s_Size);
	ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar);

	// Draw Interface callbacks
	for (auto& callback : s_InterfaceCallbacks)
	{
		callback();
	}

	if (ImGui::CollapsingHeader("ECS"))
	{
		if (ImGui::TreeNode("Entity"))
		{
			if (ImGui::TreeNode("Transform"))
			{
				ImGui::Text("Test");
				ImGui::TreePop();
				ImGui::Separator();
			}
			ImGui::TreePop();
			ImGui::Separator();
		}
	}

	ImGui::End();
	
	ImGui::ShowDemoWindow();
	/*
	ImGui::ShowUserGuide();
	*/
}

void ANKDebugInterface::RegisterInterfaceCallback(std::function<void()> callbackFunc)
{
	s_InterfaceCallbacks.push_back(callbackFunc);
}

void ANKDebugInterface::Resize(float width, float height)
{
	s_Size = ImVec2(width, height);
	s_Size.x *= s_CoverFactor.x;
	s_Size.y *= s_CoverFactor.y;
}
