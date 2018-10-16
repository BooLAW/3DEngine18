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
		////Drawing ImGui
		//if (App->scene_intro->HasObjects())
		//{
		//		if (ImGui::TreeNode(selected_go->GetName())
		//		{					
		//				if (ImGui::TreeNode("Transform"))
		//				{
		//					
		//					
		//					ImGui::TreePop();
		//				}						
		//				if (ImGui::TreeNode("Geometry"))
		//				{
		//					
		//					if (i < vertex.size())
		//					{
		//						ImGui::Spacing();
		//						ImGui::Text("Number Vertices");
		//						ImGui::SameLine();
		//						ImGui::Text("      %i", vertex[i]);
		//						ImGui::Spacing();

		//						ImGui::Text("Number Indices");
		//						ImGui::SameLine();
		//						ImGui::Text("       %i", indices[i]);
		//						ImGui::Spacing();

		//						ImGui::Text("Number Triangles");
		//						ImGui::SameLine();
		//						ImGui::Text("      %i", triangle[i]);
		//						ImGui::Spacing();

		//					}
		//					ImGui::TreePop();
		//				}
		//				if (tex_data.size() != 0) //Checking if it the mesh has a texture to display.
		//				{
		//					if (ImGui::TreeNode("Texture"))
		//					{													
		//						if (ImGui::TreeNode(tex_name[i]))
		//						{									
		//							ImGui::Spacing();
		//							ImGui::Text("Texture Coordinates:  %i ", tex_coord[i]);
		//							ImGui::Spacing();
		//							
		//							ImGui::Text("Texture Height:");
		//							ImGui::SameLine();
		//							ImGui::Text("     %i", tex_data[i]->height);
		//							ImGui::Spacing();


		//							ImGui::Text("Texture Width:", tex_data[i]->width);
		//							ImGui::SameLine();
		//							ImGui::Text("     %i", tex_data[i]->width);
		//							ImGui::Spacing();

		//							ImGui::Text("Texture ID:");
		//							ImGui::SameLine();
		//							ImGui::Text("     %i", tex_data[i]->textures_id);
		//							ImGui::Spacing();

		//							ImTextureID tex = (uint*)tex_data[i]->textures_id;
		//							ImVec2 image_size = ImGui::GetWindowSize();
		//							image_size.y = image_size.x;
		//							ImGui::Image(tex, image_size);
		//							ImGui::TreePop();
		//						}								
		//						ImGui::TreePop();
		//					}							
		//				}
		//				ImGui::TreePop();
		//			}					
		//}
	}
	ImGui::EndDock();
}
