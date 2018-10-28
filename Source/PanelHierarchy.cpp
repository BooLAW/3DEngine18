#include "PanelHierarchy.h"
#include "stdio.h"
#include "ImGui/imgui_dock.h"



PanelHierarchy::PanelHierarchy():Panel("Hierarchy")
{
	ini_width = 475;
	ini_height = 800;
	ini_pos_x = 800;
	ini_pos_y = 20;

	active = true;
}


PanelHierarchy::~PanelHierarchy()
{

}

void PanelHierarchy::Draw()
{
	ImGui::BeginDock("Hierarchy", &active, ImGuiWindowFlags_NoFocusOnAppearing);
	ImGui::Text("Total GameObjects: %d", App->scene_intro->go_list.size());
	App->scene_intro->DrawHierarchy();

	ImGui::EndDock();

}


