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
	int mass = comp_mass;
	if (owner->physbody != nullptr)
	{
		if (owner->physbody->primitive_ptr != nullptr)
		{
			mass = owner->physbody->primitive_ptr->mass;
		}
		
		if (ImGui::InputInt("Mass", &mass))
		{
			owner->physbody->primitive_ptr->mass = mass;
		}
			

		use_gravity = owner->physbody->HasGravity();
		if (ImGui::Checkbox("Use Gravity", &use_gravity))
		{
			owner->physbody->ActivateGravity(use_gravity);					
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
	else
	{
		//mass = 0;
		if (ImGui::InputInt("Mass", &mass))
		{

		}
	}
}

void ComponentRigidBody::UpdateTransform()
{
	
}

bool ComponentRigidBody::HasMoved()
{
	bool ret = false;
	return ret;
}
bool ComponentRigidBody::IsBulletStatic()
{
	return owner->physbody->GetRigidBody()->isStaticObject();
}

