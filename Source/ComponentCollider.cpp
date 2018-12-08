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
	SetName("Component Collider");

	if (owner->physbody == nullptr)
	{
		new PhysBody(owner);
	}
	type = ComponentType::COLLIDER;



}

ComponentCollider::~ComponentCollider()
{
}

bool ComponentCollider::Update()
{
	//owner->physbody->SetTransform(owner->comp_transform->trans_matrix_g.ptr());
	
	owner->physbody->SetPos(owner->comp_transform->transform->pos.x, owner->comp_transform->transform->pos.y, owner->comp_transform->transform->pos.z);
	return false;
}

void ComponentCollider::DrawInspectorInfo()
{
	static float center[3] = { 0,0,0 };
	
	if (ImGui::DragFloat3("Center##collider", center, 0.1f, -INFINITY, INFINITY))
	{
		float matrix[16];
		owner->physbody->GetTransform(matrix);
		matrix[12] =+ center[0];
		matrix[13] =+ center[1];
		matrix[14] =+ center[2];
		owner->physbody->SetTransform(matrix);
	}

	static float radius = 1.0f;
	if (ImGui::DragFloat("Radius##collider", &radius, 0.1f, -INFINITY, INFINITY))
	{
		owner->physbody->GetRigidBody()->setCcdSweptSphereRadius(radius);
	}

}

void ComponentCollider::UpdateTransform()
{
	if (HasOwner())
	{
		GameObject* owner = GetOwner();
		owner->physbody->SetTransform((float*)&owner->comp_transform->trans_matrix_g);			
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
	bool ret = owner->physbody->GetRigidBody()->isStaticObject();
	return ret;
}

