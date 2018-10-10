#include "PanelInspector.h"
#include "ModuleScene.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "Application.h"
#include "imgui_impl_sdl.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_dock.h"
#include "MeshLoader.h"
#include "ComponentMaterial.h"
#include "Material.h"


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
		if (App->input->file_droped == true)
		{
			mesh_name.clear();
			vertex.clear();
			counter = 0;
		}
		if (App->input->tex_droped == true)
		{
			tex_name.clear();
		}
		//Getting Data
		if (App->scene_intro->go_list.size() > 0 && App->input->file_droped == true || App->scene_intro->go_list.size() > 0 && App->input->tex_droped == true)
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
						uint get_triangles = ((*it)->GetMesh()->num_vertices);
						get_triangles = get_triangles / 3;
						triangle.push_back((int*)get_triangles);
						
						if ((*it)->HasTex())
						{
							ComponentMaterial* aux = (ComponentMaterial*)(*it)->GetComponent(MATERIAL);
							Material* aux_mat = aux->data;
							aux_mat->height;
						
							tex_name.push_back(App->loading_manager->tex_name_file.c_str());
							if ((*it)->GetMesh()->tex_coords != NULL)
							{
								tex_coord.push_back((int*)(*it)->GetMesh()->num_tex_coords);								
							}
						}
						counter++;
					}
				}
				
			}
			App->input->file_droped = false;
			App->input->tex_droped = false;
		}
		//Drawing ImGui
		if (App->scene_intro->go_list.size()>1)
		{
			if(ImGui::CollapsingHeader(App->scene_intro->fbx_name.c_str()))
			{
				if (ImGui::CollapsingHeader("Geometry"))
				{
					for (int i = 0; i < counter; i++)
					{
						if (ImGui::CollapsingHeader(mesh_name[i]),true)
						{
							if (i < vertex.size())
							{
								ImGui::Text("It has %i Vertex", vertex[i]);
								ImGui::Text("It has %i Triangles", triangle[i]);
							}
						}
					}
				}
				if (ImGui::CollapsingHeader("Texture"))
				{
					for (int i = 0; i < counter; i++)
					{
						if (ImGui::CollapsingHeader(tex_name[i]), true)
						{
							ImGui::Text("It has %i Texture Coordinates", tex_coord[i]);
						}
					}
				}
			}

		}

	}

	ImGui::EndDock();
}
