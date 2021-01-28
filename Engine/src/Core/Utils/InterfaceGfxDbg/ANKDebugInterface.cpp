#include "pch.h"
#include "ANKDebugInterface.h"

ImVec2 ANKDebugInterface::s_Pos = ImVec2(0.0f, 0.0f);
ImVec2 ANKDebugInterface::s_Size = ImVec2(100, 100);
ImVec2 ANKDebugInterface::s_CoverFactor = ImVec2(0.15f, 1.0f);

void ANKDebugInterface::Display()
{
	// SidePanel
	ImGui::SetNextWindowPos(s_Pos);
	ImGui::SetNextWindowSize(s_Size);
	ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar);


	if (ImGui::CollapsingHeader("Test"))
	{

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

void ANKDebugInterface::Resize(float width, float height)
{
	s_Size = ImVec2(width, height);
	s_Size.x *= s_CoverFactor.x;
	s_Size.y *= s_CoverFactor.y;
}