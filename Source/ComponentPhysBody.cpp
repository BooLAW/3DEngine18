#include "ComponentPhysBody.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ModuleSceneImGui.h"
#include "Globals.h"
#include "Camera.h"
#include "Application.h"
#include "PhysBody.h"
ComponentPhysBody::ComponentPhysBody(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component PhysBody");

	physbody = new PhysBody();
	type = ComponentType::RIGIDBODY;


}

ComponentPhysBody::~ComponentPhysBody()
{
}

bool ComponentPhysBody::Update()
{
	
	return false;
}

void ComponentPhysBody::DrawInspectorInfo()
{
	int mass = physbody->GetMass();
	if (ImGui::InputInt("Mass", &mass))
		physbody->SetMass(mass);

	bool aux_grav = physbody->HasGravity();
	if (ImGui::Checkbox("Use Gravity", &aux_grav))
	{
		physbody->ActivateGravity(aux_grav);
		App->physics->SetGravity(aux_grav);
	}

	if (ImGui::TreeNode("Constraints"))
	{
		ImGui::Text("Freeze Position"); ImGui::SameLine();
		ImGui::Checkbox("X", &physbody->const_px); ImGui::SameLine();
		ImGui::Checkbox("Y", &physbody->const_py); ImGui::SameLine();
		ImGui::Checkbox("Z", &physbody->const_pz); 

		ImGui::Text("Freeze Rotation"); ImGui::SameLine();
		ImGui::Checkbox("X", &physbody->const_rx); ImGui::SameLine();
		ImGui::Checkbox("Y", &physbody->const_ry); ImGui::SameLine();
		ImGui::Checkbox("Z", &physbody->const_rz);

		ImGui::TreePop();
	}

}

