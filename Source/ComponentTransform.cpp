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
