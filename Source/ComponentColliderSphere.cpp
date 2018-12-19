#include "ComponentColliderSphere.h"
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


ComponentColliderSphere::ComponentColliderSphere(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component Collider");
	PSphere* aux_sphere = new PSphere();
	if (owner->physbody == nullptr)
	{
		new PhysBody(owner,aux_sphere);
	}
}

ComponentColliderSphere::~ComponentColliderSphere()
{
}

bool ComponentColliderSphere::Update()
{

	//Gather the pointer with the transform matrix
	float* transform_matrix = new float[16];
	transform_matrix = owner->comp_transform->trans_matrix_g.ptr();

	//Get the identity of the body
	float current_matrix[16];
	owner->physbody->GetTransform(current_matrix);

	//Creating the final matrix where we will create the new base
	float final_matrix[16];
	for (int i = 0; i < 16; i++)
	{
		final_matrix[i] = current_matrix[i];
	}

	//Relate both matrix translation
	final_matrix[12] = transform_matrix[3] + center_offset[0];
	final_matrix[13] = transform_matrix[7] + center_offset[1];
	final_matrix[14] = transform_matrix[11] + center_offset[2];

	//Relate both matrix rotation and inverting the rotation
	final_matrix[0] = -transform_matrix[0];/**/final_matrix[1] = transform_matrix[1];/**/final_matrix[2] = transform_matrix[2];
	final_matrix[4] = transform_matrix[4];/**/final_matrix[5] = -transform_matrix[5];/**/final_matrix[6] = transform_matrix[6];
	final_matrix[8] = transform_matrix[8];/**/final_matrix[9] = transform_matrix[9];/**/final_matrix[10] = -transform_matrix[10];

	//Add the result on the object
	owner->physbody->SetTransform(final_matrix);
	
	return false;
}

void ComponentColliderSphere::DrawInspectorInfo()
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

void ComponentColliderSphere::UpdateTransform()
{
	if (HasOwner())
	{
		GameObject* owner = GetOwner();
		owner->physbody->SetTransform((float*)&owner->comp_transform->trans_matrix_g);			
	}
}

bool ComponentColliderSphere::HasMoved()
{
	bool ret = false;

	if (!IsBulletStatic())
	{
		//PAU WiP
	}

	return ret;
}
bool ComponentColliderSphere::IsBulletStatic()
{
	bool ret = owner->physbody->GetRigidBody()->isStaticObject();
	return ret;
}

