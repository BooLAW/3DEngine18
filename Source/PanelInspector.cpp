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
#include "GameObject.h"




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
		GameObject* selected_go = nullptr;
		selected_go = App->scene_intro->GetSelected();

		////Drawing ImGui
		if (selected_go != nullptr)
		{
			if (ImGui::TreeNode(selected_go->GetName()))
			{
				if (ImGui::TreeNode("Transform"))
				{


					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Geometry"))
				{
					Mesh* aux = selected_go->GetMesh();
					ImGui::Spacing();
					ImGui::Text("Number Vertices");
					ImGui::SameLine();
					ImGui::Text("      %i", aux->num_vertices);
					ImGui::Spacing();

					ImGui::Text("Number Indices");
					ImGui::SameLine();
					ImGui::Text("       %i", aux->num_indices);
					ImGui::Spacing();

					ImGui::Text("Number Texture Coords");
					ImGui::SameLine();
					ImGui::Text("      %i", aux->num_tex_coords);
					ImGui::Spacing();


					ImGui::TreePop();
				}
				if (selected_go->HasTex()) //Checking if it the mesh has a texture to display.
				{
					if (ImGui::TreeNode("Texture"))
					{
						Material* mat = selected_go->GetMaterial();

						ImGui::Spacing();
						ImGui::Text("Texture Coordinates:");
						ImGui::Spacing();

						ImGui::Text("Texture Height:");
						ImGui::SameLine();
						ImGui::Text("     %i", mat->height);
						ImGui::Spacing();


						ImGui::Text("Texture Width:");
						ImGui::SameLine();
						ImGui::Text("     %i", mat->width);
						ImGui::Spacing();

						ImGui::Text("Texture ID:");
						ImGui::SameLine();
						ImGui::Text("     %i", mat->textures_id);
						ImGui::Spacing();

						ImTextureID tex = (uint*)mat->textures_id;
						ImVec2 image_size = ImGui::GetWindowSize();
						image_size.y = image_size.x;
						ImGui::Image(tex, image_size);

						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
			
	}
	ImGui::EndDock();
}
