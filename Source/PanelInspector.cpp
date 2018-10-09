#include "PanelInspector.h"
#include "ModuleScene.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "Application.h"
#include "imgui_impl_sdl.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_dock.h"

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

	if(ImGui::BeginDock("Inspector", &active))
	{
		render_pos = ImGui::GetWindowPos();
		render_size = ImGui::GetWindowSize();
		App->scene_intro->DrawInspector();
	}

	ImGui::EndDock();
}
