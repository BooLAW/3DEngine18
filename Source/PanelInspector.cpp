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
		//Getting Data
		if (App->scene_intro->go_list.size() > 0 && App->input->file_droped == true)
		{
			for (std::vector<GameObject*>::iterator it = App->scene_intro->go_list.begin(); it < App->scene_intro->go_list.end(); it++)
			{
				if ((*it)->IsRoot() == false)
				{
					if (mesh_name.empty())
					{
						mesh_name.append((*it)->GetName());
					}

					ImGui::Text(mesh_data.c_str());
					if ((*it)->HasMesh())
					{
						Vertex = (*it)->GetMesh()->num_vertices;
					}

				}
			}
		}
		//Drawing ImGui
		if (App->input->file_droped)
		{
			ImGui::Text("Name of the mesh is: %s", mesh_name.c_str());
			if (ImGui::CollapsingHeader("Mesh"))
			{
				ImGui::Text("It has %i Vertex", Vertex);
				ImGui::Text("Size of the name %i", mesh_name.size());
			}

		}
	}

	ImGui::EndDock();
}
