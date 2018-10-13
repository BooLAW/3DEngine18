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
			indices.clear();
			triangle.clear();
			tex_name.clear();
			tex_data.clear();
			counter = 0;

		}
		
		if (App->input->tex_droped == true || App->input->file_droped == true)
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->tick_arr[56]);
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

						vertex.push_back((*it)->GetMesh()->num_vertices);
						indices.push_back((*it)->GetMesh()->num_indices);

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
		else
			App->audio->tick_arr[56] = FALSEBOOL;
		App->input->file_droped = false;
		App->input->tex_droped = false;
		
		//Drawing ImGui
		if (App->scene_intro->go_list.size()>1)
		{
			if(ImGui::TreeNode(App->scene_intro->fbx_name.c_str()))
			{
				for (int i = 0; i < counter; i++)
				{
					if (ImGui::TreeNode(mesh_name[i]))
					{					
						App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->tick_arr[i]);
						if (ImGui::TreeNode("Transform"))
						{
							App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->tick_arr[i+3]);
							ImGui::DragFloat3("Position", (float*)&positions[i]);
							ImGui::DragFloat3("Rotation", (float*)&rotations[i]);
							ImGui::DragFloat3("Scale", (float*)&scales[i]);
							ImGui::TreePop();
						}
						else
							App->audio->tick_arr[i+3] = FALSEBOOL;
						if (ImGui::TreeNode("Geometry"))
						{
							App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->tick_arr[i+6]);
							if (i < vertex.size())
							{
								ImGui::Spacing();
								ImGui::Text("Number Vertices");
								ImGui::SameLine();
								ImGui::Text("      %i", vertex[i]);
								ImGui::Spacing();

								ImGui::Text("Number Indices");
								ImGui::SameLine();
								ImGui::Text("       %i", indices[i]);
								ImGui::Spacing();

								ImGui::Text("Number Triangles");
								ImGui::SameLine();
								ImGui::Text("      %i", triangle[i]);
								ImGui::Spacing();

							}
							ImGui::TreePop();
						}
						else
							App->audio->tick_arr[i+6] = FALSEBOOL;
						if (tex_data.size() != 0) //Checking if it the mesh has a texture to display.
						{
							if (ImGui::TreeNode("Texture"))
							{					
								App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->tick_arr[i+9]);
								if (ImGui::TreeNode(tex_name[i]))
								{
									App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->tick_arr[i+12]);
									ImGui::Spacing();
									ImGui::Text("Texture Coordinates:  %i ", tex_coord[i]);
									ImGui::Spacing();
									
									ImGui::Text("Texture Height:");
									ImGui::SameLine();
									ImGui::Text("     %i", tex_data[i]->height);
									ImGui::Spacing();


									ImGui::Text("Texture Width:", tex_data[i]->width);
									ImGui::SameLine();
									ImGui::Text("     %i", tex_data[i]->width);
									ImGui::Spacing();

									ImGui::Text("Texture ID:");
									ImGui::SameLine();
									ImGui::Text("     %i", tex_data[i]->textures_id);
									ImGui::Spacing();

									ImTextureID tex = (uint*)tex_data[i]->textures_id;
									ImVec2 image_size = ImGui::GetWindowSize();
									image_size.y = image_size.x;
									ImGui::Image(tex, image_size);
									ImGui::TreePop();
								}
								else
									App->audio->tick_arr[i+12] = FALSEBOOL;
								ImGui::TreePop();
							}
							else
								App->audio->tick_arr[i+9] = FALSEBOOL;
						}
						ImGui::TreePop();
					}
					else
						App->audio->tick_arr[i] = FALSEBOOL;
				}
				ImGui::TreePop();
			}
			

		}
	}

	ImGui::EndDock();
}
