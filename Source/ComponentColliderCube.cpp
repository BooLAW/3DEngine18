#include "ComponentColliderCube.h"
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


ComponentColliderCube::ComponentColliderCube(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component Collider");
	type = ComponentType::COLLIDERCUBE;
	PCube* aux_cube = new PCube();
	if (owner->physbody == nullptr)
	{
		new PhysBody(owner, aux_cube);
	}
}

ComponentColliderCube::~ComponentColliderCube()
{
}

bool ComponentColliderCube::Update()
{	
	//Gather the pointer with the transform matrix
	float* transform_matrix = new float[16];
	transform_matrix = owner->comp_transform->trans_matrix_g.ptr();

	//Get the identity of the body
	float current_pmatrix[16];
	owner->physbody->GetTransform(current_pmatrix);

	//Creating the final matrix where we will create the new base
	float final_pmatrix[16];
	for (int i = 0; i < 16; i++)
	{
		final_pmatrix[i] = current_pmatrix[i];
	}

	if (App->state == stopped)
	{
		
		float3x3 rot = {
			transform_matrix[0],transform_matrix[1],transform_matrix[2],
			transform_matrix[4],transform_matrix[5],transform_matrix[6],
			transform_matrix[8],transform_matrix[9],transform_matrix[10] 
		};
					
		rot.Transpose();

		//Relate both matrix rotation and inverting the rotation
		final_pmatrix[0] = rot[0][0];		final_pmatrix[1] = rot[0][1];			final_pmatrix[2] = rot[0][2];
		final_pmatrix[4] = rot[1][0];		final_pmatrix[5] = rot[1][1];			final_pmatrix[6] = rot[1][2];
		final_pmatrix[8] = rot[2][0];		final_pmatrix[9] = rot[2][1];			final_pmatrix[10] = rot[2][2];

		//Relate both matrix translation
		final_pmatrix[12] = transform_matrix[3] + center_offset[0];
		final_pmatrix[13] = transform_matrix[7] + center_offset[1];
		final_pmatrix[14] = transform_matrix[11] + center_offset[2];

		//Add the result on the object
		owner->physbody->SetTransform(final_pmatrix);
	}
	if (App->state == playing)
	{
		////Creating the final matrix where we will create the new base
		//float final_matrix[16];
		//for (int i = 0; i < 16; i++)
		//{
		//	final_matrix[i] = current_pmatrix[i];
		//}

		//float4x4 final_matrix4x4;
		//final_matrix4x4[0][0] = -transform_matrix[0];	final_matrix4x4[0][1] = transform_matrix[1];	final_matrix4x4[0][2] = transform_matrix[2];	final_matrix4x4[0][3] = 1.0f;
		//final_matrix4x4[1][0] = transform_matrix[4];	final_matrix4x4[1][1] = -transform_matrix[5];	final_matrix4x4[1][2] = transform_matrix[6];	final_matrix4x4[1][3] = 1.0f;
		//final_matrix4x4[2][0] = transform_matrix[8];	final_matrix4x4[2][1] = transform_matrix[9];	final_matrix4x4[2][2] = -transform_matrix[10];	final_matrix4x4[2][3] = 1.0f;
		//final_matrix4x4[3][0] = transform_matrix[3];	final_matrix4x4[3][1] = transform_matrix[7];	final_matrix4x4[3][2] = transform_matrix[11];	final_matrix4x4[3][3] = 1.0f;

		//owner->comp_transform->SetLocalMatrix(final_matrix4x4);
	}

	
	
	return false;
}

void ComponentColliderCube::DrawInspectorInfo()
{
	
	if (ImGui::DragFloat3("Center##collider", center_offset, 0.1f, -INFINITY, INFINITY)) 
	{
		Update();
	}

	static float dimensions = 1.0f;
	if (ImGui::DragFloat("Dimensions##collider", &dimensions, 0.01f, 1, 10))
	{
		
	}

}

void ComponentColliderCube::UpdateTransform()
{
	if (HasOwner())
	{
		GameObject* owner = GetOwner();
		owner->physbody->SetTransform((float*)&owner->comp_transform->trans_matrix_g);			
	}
}

bool ComponentColliderCube::HasMoved()
{
	bool ret = false;

	if (!IsBulletStatic())
	{
		//PAU WiP
	}

	return ret;
}
bool ComponentColliderCube::IsBulletStatic()
{
	bool ret = owner->physbody->GetRigidBody()->isStaticObject();
	return ret;
}

