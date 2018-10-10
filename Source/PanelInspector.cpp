#include "PanelInspector.h"
#include "ModuleScene.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "Application.h"
#include "imgui_impl_sdl.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_dock.h"
#include "MeshLoader.h"


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
		//Reset
		if (last_fbx != App->scene_intro->fbx_name.c_str() || App->input->file_droped == true)
		{
			mesh_name.clear();
			vertex.clear();
			counter = 0;
		}
		//Getting Data
		if (App->scene_intro->go_list.size() > 0 && App->input->file_droped == true)
		{
			for (std::vector<GameObject*>::iterator it = App->scene_intro->go_list.begin(); it < App->scene_intro->go_list.end(); it++)
			{
				if ((*it)->IsRoot() == false)
				{
					if ((*it)->GetName())
					if (mesh_name.size() <= (*it)->num_meshes)
					{
						mesh_name.push_back((*it)->GetName());
	
					}
					if ((*it)->HasMesh())
					{
						vertex.push_back((int*)(*it)->GetMesh()->num_vertices); 
						counter++;
					}
					last_fbx = App->scene_intro->fbx_name.c_str();
				}
				
			}
			App->input->file_droped = false;
		}
		//Drawing ImGui
		if (App->scene_intro->go_list.size()>1)
		{
			if(ImGui::CollapsingHeader(App->scene_intro->fbx_name.c_str()))
			{
				for (int i = 0; i < counter; i++)
				{
					if (ImGui::CollapsingHeader(mesh_name[i]))
					{
						if (i < vertex.size())
						{
							ImGui::Text("It has %i Vertex", vertex[i]);
						}
					}
				}
			}

		}

	}

	ImGui::EndDock();
}
