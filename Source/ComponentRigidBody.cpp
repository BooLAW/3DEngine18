#include "ComponentRigidBody.h"
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


ComponentRigidBody::ComponentRigidBody(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component RigidBody");

	if (owner->physbody == nullptr)
	{
		owner->physbody = new PhysBody(owner);
	}
	type = ComponentType::RIGIDBODY;


}

ComponentRigidBody::~ComponentRigidBody()
{
}

bool ComponentRigidBody::Update()
{
	
	return false;
}

void ComponentRigidBody::DrawInspectorInfo()
{
	int mass = owner->physbody->GetMass();
	if (ImGui::InputInt("Mass", &mass))
		owner->physbody->SetMass(mass);

	bool aux_grav = owner->physbody->HasGravity();
	if (ImGui::Checkbox("Use Gravity", &aux_grav))
	{
		owner->physbody->ActivateGravity(aux_grav);
	}

	if (ImGui::TreeNode("Constraints"))
	{
		ImGui::Text("Freeze Position"); ImGui::SameLine();
		ImGui::Checkbox("X", &owner->physbody->const_px); ImGui::SameLine();
		ImGui::Checkbox("Y", &owner->physbody->const_py); ImGui::SameLine();
		ImGui::Checkbox("Z", &owner->physbody->const_pz);

		ImGui::Text("Freeze Rotation"); ImGui::SameLine();
		ImGui::Checkbox("X", &owner->physbody->const_rx); ImGui::SameLine();
		ImGui::Checkbox("Y", &owner->physbody->const_ry); ImGui::SameLine();
		ImGui::Checkbox("Z", &owner->physbody->const_rz);

		ImGui::TreePop();
	}

}

void ComponentRigidBody::UpdateTransform()
{
	//if (HasOwner())
	//{
	//	GameObject* owner = GetOwner();
	//	owner->physbody->SetTransform((float*)&owner->comp_transform->trans_matrix_g);
	//}
}

bool ComponentRigidBody::HasMoved()
{
	bool ret = false;

	if (!IsBulletStatic())
	{
		//PAU WiP
	}

	return ret;
}
bool ComponentRigidBody::IsBulletStatic()
{
	bool ret = owner->physbody->GetRigidBody()->isStaticObject();
	return ret;
}
