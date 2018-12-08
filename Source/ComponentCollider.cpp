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
	//TODO Make the same with a matrix
	//owner->physbody->SetTransform(owner->comp_transform->trans_matrix_g.ptr());	

	owner->physbody->SetPos(owner->comp_transform->transform->pos.x + center_offset[0], owner->comp_transform->transform->pos.y + center_offset[1], owner->comp_transform->transform->pos.z + center_offset[2]);
	return false;
}

void ComponentCollider::DrawInspectorInfo()
{
	
	if (ImGui::DragFloat3("Center##collider", center_offset, 0.1f, -INFINITY, INFINITY)) 
	{
		Update();
	}

	static float radius = 1.0f;
	if (ImGui::DragFloat("Radius##collider", &radius, 0.01f, 1, 10))
	{
		
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

