#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "Globals.h"
#include "Camera.h"

ComponentTransform::ComponentTransform(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component Transform");
	transform = new Transform();

	type = ComponentType::TRANSFORM;

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
	if(owner->GetParent() == nullptr)//IS ROOT
		trans_matrix_g = float4x4::FromTRS(transform->pos, transform->rot, transform->scale);
	else
	{
		ComponentTransform* owner_transform = (ComponentTransform*)GetOwner()->GetParent()->GetComponent(ComponentType::TRANSFORM);
		trans_matrix_g = owner_transform->trans_matrix_g*float4x4::FromTRS(transform->pos, transform->rot, transform->scale);
	}
	owner->RecursiveUpdateTransformChilds();
}

void ComponentTransform::DrawInspectorInfo()
{
	bool reset_transform = false;
	float pos[3] = { transform->pos.x,transform->pos.y,transform->pos.z };
	float rot[3] = { transform->rot.ToEulerXYZ().x*RADTODEG, transform->rot.ToEulerXYZ().y*RADTODEG, transform->rot.ToEulerXYZ().z*RADTODEG };
	float scale[3] = { transform->scale.x,transform->scale.y,transform->scale.z };

	if (ImGui::Button("Identity Reset"))
		reset_transform = !reset_transform;

	ImGui::DragFloat3("Position##transform", pos, 0.1f, -INFINITY, INFINITY);
	ImGui::DragFloat3("Rotation##transform", rot, 0.1f, -360, 360);
	ImGui::DragFloat3("Scale##transform", scale, 0.1f, 1, INFINITY);
	
	//Update Transform
	updated_transform = false;
	if (!owner->IsStatic())
	{
		if (pos[0] != transform->pos.x || pos[1] != transform->pos.y || pos[2] != transform->pos.z)
		{
			transform->SetPosition(pos[0], pos[1], pos[2]);
			updated_transform = true;
		}
		Quat aux = Quat::FromEulerXYZ(rot[0] * DEGTORAD, rot[1] * DEGTORAD, rot[2] * DEGTORAD);
		if (!transform->rot.Equals(aux));
		{
			transform->SetRotation(aux);
			updated_transform = true;
		}
		if (pos[0] != transform->scale.x || pos[1] != transform->scale.y || pos[2] != transform->scale.z)
		{
			transform->SetScale(scale[0], scale[1], scale[2]);
			updated_transform = true;
		}
		if (updated_transform)
		{
			UpdateTransformValues();
			if(owner->HasMesh())
				owner->RecalculateBoundingBox(owner);
		}
		if (reset_transform)
		{
			transform->SetPosition(float3::zero.x, float3::zero.y, float3::zero.z);
			Quat aux = Quat::FromEulerXYZ(float3::zero.x * DEGTORAD, float3::zero.y * DEGTORAD, float3::zero.z * DEGTORAD);

			transform->SetRotation(aux);

			transform->SetScale(float3::one.x, float3::one.y, float3::one.z);

		}
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

	transform->pos = new_pos;
	trans_matrix_l.Set(float4x4::FromTRS(transform->pos, transform->rot, transform->scale));
	updated_transform = true;

}

void ComponentTransform::SetGlobalPos(const float3 & new_pos)
{
	ComponentMesh* mesh;

	if (owner != nullptr)//it should never happen
	{
		mesh = (ComponentMesh*)owner->GetComponent(ComponentType::MESH);
		//implement static checking
	}
	else
		CONSOLE_LOG_WARNING("Component Transform has no owner");

	transform->pos = new_pos;
	trans_matrix_g.Set(float4x4::FromTRS(transform->pos, transform->rot, transform->scale));
	updated_transform = true;
}

void ComponentTransform::SetTransform(float3 pos, Quat rot,float3 scale)
{
	this->transform->pos = pos;
	this->transform->rot = rot;
	this->transform->scale = scale;

}

Transform* ComponentTransform::GetTransform() const
{
	return transform;
}
