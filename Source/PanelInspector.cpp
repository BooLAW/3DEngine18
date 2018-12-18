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
#include "ComponentCamera.h"
#include "ComponentRigidBody.h"
#include "ComponentPlayerController.h"
#include "ComponentColliderSphere.h"
#include "ComponentColliderCube.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"




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
			ImGui::Text("Name: %s", selected_go->GetName()); 
			
			ImGui::Separator();
		
				bool static_go = selected_go->IsStatic();
				if (ImGui::Checkbox("Static", &static_go))
					selected_go->SetStatic(static_go);
				ImGui::SameLine();
				bool active = selected_go->IsActive();
				if (ImGui::Checkbox("Active", &active))
					selected_go->SetActive(active);
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					App->scene_intro->DeleteGameObject(selected_go);
				}
				static ImGuiComboFlags flags = 0;
				const char* items[] = { "NONE","RigidBody","Collider Sphere", "Collider Cube", "Camera","Material WiP", "Mesh WiP" };
				static const char* item_current = items[0];            // Here our selection is a single pointer stored outside the object.
				if (ImGui::BeginCombo("Create", item_current, flags)) // The second parameter is the label previewed before opening the combo.
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (item_current == items[n]);
						if (ImGui::Selectable(items[n], is_selected))
						{
							item_current = items[n];
							//CREATE EACH ELEMENT
							if (items[n] == "RigidBody")
							{
								ComponentRigidBody* new_comp = new ComponentRigidBody(selected_go);
								selected_go->PushComponent(new_comp);
							}
							else if (items[n] == "Camera")
							{
								if (selected_go->HasCam())
									continue;
								ComponentCamera* new_comp = new ComponentCamera();
								new_comp->SetOwner(selected_go);
								selected_go->PushComponent(new_comp);									
							}
							else if (items[n] == "Collider Sphere")
							{
								if (selected_go->HasColliderSphere())
									continue;
								ComponentColliderSphere* new_comp = new ComponentColliderSphere(selected_go);
								selected_go->PushComponent(new_comp);
							}
							else if (items[n] == "Collider Cube")
							{
								if (selected_go->HasColliderCube())
									continue;
								ComponentColliderCube* new_comp = new ComponentColliderCube(selected_go);
								selected_go->PushComponent(new_comp);
							}
							
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
					}
					ImGui::EndCombo();
				}
				ImGui::Separator();
				

			if (selected_go != nullptr)
			{
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					selected_go->comp_transform->DrawInspectorInfo();
				}

				//Check if has Mesh
				if (selected_go->HasMesh())
				{
					if (ImGui::CollapsingHeader("Geometry"))
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
						bool draw_n = selected_go->GetDrawNormals();
						if(ImGui::Checkbox("Draw Normals", &draw_n))
							selected_go->SetDrawNormals(draw_n);
					}
				}
				//Check if it has Materials and then it draws it
				if (selected_go->HasTex()) //Checking if it the mesh has a texture to display.
				{
					if (ImGui::CollapsingHeader("Texture"))
					{
						Material* mat = selected_go->GetMaterial();

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
						image_size.x = image_size.x / 2;
						image_size.y = image_size.x;
						ImGui::Image(tex, image_size);
					}
				}

				if (selected_go->HasCam()) //Checking if it the mesh has a texture to display.
				{
					if (ImGui::CollapsingHeader("Camera"))
					{
						Camera* cam = selected_go->GetCamera();
						//AR
						bool frust = cam->IsFrustumActive();
						if (ImGui::Checkbox("Show Frustum", &frust))
							cam->ActivateFrustum(frust);
					
						ImGui::Separator();
						//Frustum 
						ImGui::Text("Frustum Info");
						ImGui::Spacing();
						//Far Plane
						float far_aux = cam->GetFarPlane();
						if (ImGui::InputFloat("Far Plane", &far_aux,10.0f))
							cam->SetFarPlane(far_aux);
						ImGui::Spacing();
						//Near Plane
						float near_aux = cam->GetNearPlane();
						if (ImGui::InputFloat("Near Plane", &near_aux,0.5f))
							cam->SetNearPlane(near_aux);
						ImGui::Spacing();
						//FOV
						float fov_aux = cam->GetVerticalFOV();
						if (ImGui::InputFloat("FOV", &fov_aux,5.0f))
							cam->SetFOV(fov_aux);
						ImGui::Spacing();
						float a_ratio = cam->GetAspectRatio();
						if (ImGui::InputFloat("Aspect Ratio", &a_ratio, 10.0f))
							cam->SetAspectRatio(a_ratio);
						ImGui::Spacing();

					}
				}

				//Check if it has PhysBody
				if (selected_go->HasRigidBody())
				{
					ComponentRigidBody* rb = selected_go->GetRigidBody();
					if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen))
					{
						selected_go->GetComponent(RIGIDBODY)->DrawInspectorInfo();
					}
				}
				//Check if it has Collider Sphere
				if (selected_go->HasColliderSphere())
				{
					ComponentColliderSphere* rb = selected_go->GetColliderSphere();
					if (ImGui::CollapsingHeader("Collider Sphere", ImGuiTreeNodeFlags_DefaultOpen))
					{
						selected_go->GetComponent(COLLIDERSPHERE)->DrawInspectorInfo();
					}
				}
				//Check if it has Collider Cube
				if (selected_go->HasColliderCube())
				{
					ComponentColliderCube* rb = selected_go->GetColliderCube();
					if (ImGui::CollapsingHeader("Collider Cube", ImGuiTreeNodeFlags_DefaultOpen))
					{
						selected_go->GetComponent(COLLIDERCUBE)->DrawInspectorInfo();
					}
				}
				if (selected_go->HasController())
				{
					if (ImGui::CollapsingHeader("Player Controller", ImGuiTreeNodeFlags_DefaultOpen))
					{
						selected_go->GetComponent(CONTROLLER)->DrawInspectorInfo();
					}

				}
				
				if (selected_go->HasConstraintJoint()) {
					if (ImGui::CollapsingHeader("Constraint", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Text("Type: %d", "Point to Point");
						
					}
				}
			}			
		}
		else
			ImGui::Text("Select a GameObject in the Hierarchy");
			
	}
	ImGui::EndDock();
}
