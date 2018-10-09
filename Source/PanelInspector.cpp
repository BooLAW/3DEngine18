#include "PanelInspector.h"
#include "ModuleSceneIntro.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ModuleSceneIntro.h"
#include "Application.h"


PanelInspector::PanelInspector(): Panel("Inspector")
{
	ini_pos_x = 800;
	ini_pos_y = 20;
	ini_width = 475;
	ini_height = 800;
	render_pos = { 800,20 };
	render_size = { 800,20 };
	active = true;
}


PanelInspector::~PanelInspector()
{
}

void PanelInspector::MeshComponentInfo(GameObject * active_GO)
{
	
}

void PanelInspector::Draw()
{
	ImGui::Begin("Inspector", &active, ImGuiWindowFlags_NoFocusOnAppearing);
	render_pos = ImGui::GetWindowPos();
	render_size = ImGui::GetWindowSize();
	App->scene_intro->DrawInspector();

	ImGui::End();
}
