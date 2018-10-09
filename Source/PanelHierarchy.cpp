#include "PanelHierarchy.h"
#include "stdio.h"



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
	ImGui::Begin("Hierarchy", &active, ImGuiWindowFlags_NoFocusOnAppearing);
	render_pos = ImGui::GetWindowPos();
	render_size = ImGui::GetWindowSize();
	App->scene_intro->DrawHierarchy();

	ImGui::End();

}


