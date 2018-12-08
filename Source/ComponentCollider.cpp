#include "ComponentCollider.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ModuleSceneImGui.h"
#include "Globals.h"
#include "Camera.h"
#include "Application.h"
#include "PhysBody.h"
#include "Transform.h"
#include "ComponentTransform.h"


ComponentCollider::ComponentCollider(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component PhysBody");

	physbody = new PhysBody();
	type = ComponentType::COLLIDER;


}

ComponentCollider::~ComponentCollider()
{
}

bool ComponentCollider::Update()
{
	
	return false;
}

void ComponentCollider::DrawInspectorInfo()
{
	int mass = physbody->GetMass();
	if (ImGui::InputInt("Mass", &mass))
		physbody->SetMass(mass);

	bool aux_grav = physbody->HasGravity();
	if (ImGui::Checkbox("Use Gravity", &aux_grav))
	{
		physbody->ActivateGravity(aux_grav);
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

void ComponentCollider::UpdateTransform()
{
	if (HasOwner())
	{
			GameObject* owner = GetOwner();
			physbody->SetTransform((float*)&owner->comp_transform->trans_matrix_g);			
	}
}

bool ComponentCollider::HasMoved()
{
	bool ret = false;

	if (!IsBulletStatic())
	{
		//PAU WiP
	}

	return ret;
}
bool ComponentCollider::IsBulletStatic()
{
	bool ret = physbody->GetRigidBody()->isStaticObject();
	return ret;
}

