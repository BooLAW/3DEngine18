#include "PanelInspector.h"
#include "ModuleScene.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "Application.h"
#include "imgui_impl_sdl.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_dock.h"
#include "MeshLoader.h"
#include "ComponentTransform.h"
#include "Transform.h"




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
		if (App->input->file_droped == true || App->input->tex_droped == true)
		{
			//clear variables
			mesh_name.clear();
			vertex.clear();
			tex_name.clear();
			tex_data.clear();
			triangle.clear();
			counter = 0;

		}
		
		if (App->input->tex_droped == true || App->input->file_droped == true)
		{
			//Getting Data
			for (std::vector<GameObject*>::iterator it = App->scene_intro->go_list.begin(); it < App->scene_intro->go_list.end(); it++)
			{
				if ((*it)->IsRoot() == false)
				{
					
					if ((*it)->GetName() != nullptr)
					{
						mesh_name.push_back((*it)->GetName());
	
					}
					if ((*it)->transform != nullptr)
					{
						positions.push_back((*it)->transform->transform.pos);
						rotations.push_back((*it)->transform->transform.rot.ToEulerXYZ() *RADTODEG);
						scales.push_back((*it)->transform->transform.scale);

					}
					if ((*it)->HasMesh())
					{
						
						vertex.push_back((*it)->GetMesh()->num_indices); 
						uint get_triangles = ((*it)->GetMesh()->num_vertices);
						
						get_triangles = get_triangles / 3;
						triangle.push_back(get_triangles);										
						counter++;
					}
				}
			}
			//Getting Textures
			for (std::vector<GameObject*>::iterator it = App->scene_intro->go_list.begin(); it < App->scene_intro->go_list.end(); it++)
			{
				if ((*it)->HasTex())
				{
					ComponentMaterial* aux = (ComponentMaterial*)(*it)->GetComponent(MATERIAL);
					Material* aux_mat = aux->data;

					tex_data.push_back(aux_mat);

					tex_name.push_back(App->loading_manager->tex_name_file.c_str());
					if ((*it)->GetMesh() != nullptr)
					{
						tex_coord.push_back((int*)(*it)->GetMesh()->num_tex_coords);
					}
				}
			}
			
		}

		App->input->file_droped = false;
		App->input->tex_droped = false;
		//Drawing ImGui
		if (App->scene_intro->go_list.size()>1)
		{
			if(ImGui::CollapsingHeader(App->scene_intro->fbx_name.c_str()))
			{
				if (ImGui::CollapsingHeader("Transform"))
				{
					for (int i = 0; i < counter; i++)
					{
						if (ImGui::CollapsingHeader(mesh_name[i]), true)
						{
							ImGui::DragFloat3("Position", (float*)&positions[i]);
							ImGui::DragFloat3("Rotation", (float*)&rotations[i]);
							ImGui::DragFloat3("Scale", (float*)&scales[i]);

						}
					}
				}
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
				if (tex_data.size() != 0) //Checking if it the mesh has a texture to display.
				{
					if (ImGui::CollapsingHeader("Texture"))
					{
						for (int i = 0; i < counter; i++)
						{
							if (ImGui::CollapsingHeader(tex_name[i]), true)
							{
								ImGui::Text("Texture Coordinates: TAB %i ", tex_coord[i]);
								ImGui::Text("Height: %i", tex_data[i]->height);
								ImGui::Text("Width: %i", tex_data[i]->width);
								ImGui::Text("It has id %i", tex_data[i]->textures_id);

								ImTextureID tex = (uint*)tex_data[i]->textures_id;
								ImVec2 image_size = ImGui::GetWindowSize();
								image_size.y = image_size.x;
								ImGui::Image(tex, image_size);

							}
						}
					}
				}
			}
		}
	}

	ImGui::EndDock();
}
