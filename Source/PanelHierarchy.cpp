#include "PanelHierarchy.h"
#include "stdio.h"
#include "ImGui/imgui_dock.h"
#include "GameObject.h"
#include "ComponentColliderSphere.h"
#include "ComponentColliderCube.h"
#include "ComponentTransform.h"





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
	ImGui::BeginDock("Hierarchy", &active, ImGuiWindowFlags_NoFocusOnAppearing);

	static ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft;

	const char* items[] = { "NONE", "Empty Child","Sphere","Cube", "Plane WiP","Main Camera","Distance Joint"};
	static const char* item_current = items[0];            // Here our selection is a single pointer stored outside the object.
	if (ImGui::BeginCombo("Add GO", item_current, flags)) // The second parameter is the label previewed before opening the combo.
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			bool is_selected = (item_current == items[n]);
			if (ImGui::Selectable(items[n], is_selected))
			{
				item_current = items[n];
				//CREATE EACH ELEMENT
				if (items[n] == "Empty Child")
				{

				}
				else if (items[n] == "Empty Child")
				{
					//App->scene_intro->CreateNewGameObject();
				}
				else if (items[n] == "Sphere")
				{
					PSphere* test = new PSphere();
					test->radius = 1;	
					test->isCollider = false;
					test->has_primitive_render = true;

					GameObject* tmp_go = new GameObject();
					std::string new_name = "Sphere (";

					//Add number
					new_name += std::to_string(id_new_sphere);
					new_name.append(")");
					id_new_sphere++;

					tmp_go->SetName(new_name.c_str());
					tmp_go->SetParent(App->scene_intro->scene_root);					
					tmp_go->comp_transform->SetGlobalPos(float3(0, 10, 0));
					tmp_go->SetSelected(true);
					tmp_go->comp_transform->UpdateTransformValues();					
					

					ComponentColliderSphere* cmp_collider = new ComponentColliderSphere(tmp_go, test);
					
					tmp_go->PushComponent((Component*)cmp_collider);


					App->scene_intro->go_list.push_back(tmp_go);
				}
				else if (items[n] == "Cube")
				{
					PCube* test = new PCube();
					test->dimensions = { 5,5,5 };

					GameObject* tmp_go = new GameObject();
					std::string new_name = "Cube (";

					//Add number
					new_name += std::to_string(id_new_cube);
					new_name.append(")");
					id_new_cube++;

					tmp_go->SetName(new_name.c_str());
					tmp_go->SetParent(App->scene_intro->scene_root);
					tmp_go->comp_transform->SetGlobalPos(float3(0, 15, 0));
					tmp_go->SetSelected(true);
					tmp_go->comp_transform->UpdateTransformValues();


					ComponentColliderCube* cmp_collider = new ComponentColliderCube(tmp_go);

					tmp_go->PushComponent((Component*)cmp_collider);


					App->scene_intro->go_list.push_back(tmp_go);
				}
				else if (items[n] == "Main Camera")
				{
					if (App->scene_intro->main_camera_go == nullptr)
					{
						App->scene_intro->NewMainCamera();
					}
				}
				else if (items[n] == "Distance Joint")
				{
					App->scene_intro->CreateDistanceJoint();
				}
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
		}
		ImGui::EndCombo();
	}
	
	ImGui::Text("Total GameObjects: %d", App->scene_intro->go_list.size()/*-1*/); //Why there is a -1 here?
	
	ImGui::Separator();
	App->scene_intro->DrawHierarchy();

	ImGui::EndDock();

}


