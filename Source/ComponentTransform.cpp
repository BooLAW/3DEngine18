#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "Globals.h"


ComponentTransform::ComponentTransform(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component Transform");

	type = ComponentType::TANSFORM;

}

ComponentTransform::~ComponentTransform()
{
}

bool ComponentTransform::Update()
{
	return false;
}

void ComponentTransform::UpdateTransformValues()
{
	if(owner->GetParent() == nullptr)
		trans_matrix_g = float4x4::FromTRS(transform.pos, transform.rot, transform.scale);
	else
	{
		ComponentTransform* owner_transform = (ComponentTransform*)GetOwner()->GetParent()->GetComponent(ComponentType::TANSFORM);
		trans_matrix_g = owner_transform->trans_matrix_g*float4x4::FromTRS(transform.pos, transform.rot, transform.scale);
	}
	//Make a function to update recursively all the childs and components related
	owner->RecursiveUpdateTransformChilds();
}

void ComponentTransform::DrawInspectorInfo()
{
	float pos[3] = { transform.pos.x,transform.pos.y,transform.pos.z };
	float rot[3] = { transform.rot.ToEulerXYZ().x*RADTODEG, transform.rot.ToEulerXYZ().y*RADTODEG, transform.rot.ToEulerXYZ().z*RADTODEG };
	float scale[3] = { transform.scale.x,transform.scale.y,transform.scale.z };


		ImGui::DragFloat3("Position##transform", pos, 0.1f, -INFINITY, INFINITY);
		ImGui::DragFloat3("Rotation##transform", rot, 0.1f, -360, 360);
		ImGui::DragFloat3("Scale##transform", scale, 0.1f, 1, INFINITY);
	
	//Update Transform
	if (!owner->IsStatic())
	{
		if (pos[0] != transform.pos.x || pos[1] != transform.pos.y || pos[2] != transform.pos.z)
		{
			transform.SetPosition(pos[0], pos[1], pos[2]);
			updated_transform = true;
		}
		if (updated_transform)
			UpdateTransformValues();
	}
}
void ComponentTransform::SetLocalPos(const float3 & new_pos)
{
	ComponentMesh* mesh;

	if (owner != nullptr)//it should never happen
	{
		mesh = (ComponentMesh*)owner->GetComponent(ComponentType::MESH);
		//implement static checking
	}
	else
		CONSOLE_LOG_WARNING("Component Transform has no owner");

	transform.pos = new_pos;
	trans_matrix_l.Set(float4x4::FromTRS(transform.pos, transform.rot, transform.scale));
	updated_transform = true;

}

void ComponentTransform::SetGlobalPos(const float3 & new_pos)
{
}

void ComponentTransform::SetTransform(float3 pos, Quat rot,float3 scale)
{
	this->transform.pos = pos;
	this->transform.rot = rot;
	this->transform.scale = scale;

}

Transform ComponentTransform::GetTransform() const
{
	return transform;
}
