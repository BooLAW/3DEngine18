#include "PanelToolButtons.h"
#include "ModuleSceneImGui.h"
#include "Application.h"
#include "Globals.h"

PanelToolButtons::PanelToolButtons():Panel("ToolButtons")
{
}

void PanelToolButtons::Draw()
{
	if (ImGui::BeginDock("Tool Buttons", &active))
	{
		if (ImGui::Button("TRANS"))
			App->imgui->guizmo_operation = TRANSLATE;
		ImGui::SameLine();
		if (ImGui::Button("ROTATE"))
			App->imgui->guizmo_operation = ROTATE;
		ImGui::SameLine();
		if (ImGui::Button("SCALE"))
			App->imgui->guizmo_operation = SCALE;
	}
	ImGui::EndDock();
}


PanelToolButtons::~PanelToolButtons()
{
}
