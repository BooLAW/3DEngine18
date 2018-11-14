#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "Globals.h"
#include "Camera.h"

ComponentTransform::ComponentTransform(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component Transform");
	transform.pos = float3(0.0f, 0.0f, 0.0f);
	transform.rot = Quat::identity;
	transform.rot_euler = float3(0.0f, 0.0f, 0.0f);
	transform.scale = float3(1.0f, 1.0f, 1.0f);

	CalculateLocalMatrix();

	trans_matrix_g = float4x4::identity;
	updated_transform = true;
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
	trans_matrix_g = float4x4::identity;
	trans_matrix_l = float4x4::FromTRS(transform.pos, transform.rot, transform.scale);

	if(owner->GetParent() == nullptr)//IS ROOT
		trans_matrix_g = trans_matrix_l;
	else
	{
		ComponentTransform* owner_transform = (ComponentTransform*)GetOwner()->GetParent()->GetComponent(ComponentType::TRANSFORM);
		trans_matrix_g = owner_transform->trans_matrix_g * trans_matrix_l;
	}
	if(owner->HasChilds())	
		owner->RecursiveUpdateTransformChilds();
}

void ComponentTransform::DrawInspectorInfo()
{
	bool reset_transform = false;

	float pos[3] = { transform.pos.x,transform.pos.y,transform.pos.z };
	float rot[3] = { transform.rot.ToEulerXYZ().x * RADTODEG, transform.rot.ToEulerXYZ().y * RADTODEG, transform.rot.ToEulerXYZ().z *RADTODEG };
	float scale[3] = { transform.scale.x,transform.scale.y,transform.scale.z };
	transform.rot_euler = (float3)rot;

	if (ImGui::Button("Identity Reset"))
		reset_transform = !reset_transform;

	ImGui::DragFloat3("Position##transform", pos, 0.1f, -INFINITY, INFINITY);
	ImGui::DragFloat3("Rotation##transform", rot, 0.1f, -360, 360);
	ImGui::DragFloat3("Scale##transform", scale, 0.1f, 1, INFINITY);
	
	//Update Transform
	updated_transform = false;
	if (!owner->IsStatic())
	{
		if (pos[0] != transform.pos.x || pos[1] != transform.pos.y || pos[2] != transform.pos.z)
		{
			transform.SetPosition(pos[0], pos[1], pos[2]);
			updated_transform = true;
		}
		Quat aux = Quat::FromEulerXYZ(rot[0] * DEGTORAD, rot[1] * DEGTORAD, rot[2] * DEGTORAD);
		if (!transform.rot_euler.Equals(rot[0],rot[1],rot[2]))
		{
			transform.SetRotation(aux);
			updated_transform = true;
		}
		if (scale[0] != transform.scale.x || scale[1] != transform.scale.y || scale[2] != transform.scale.z)
		{
			transform.SetScale(scale[0], scale[1], scale[2]);
			updated_transform = true;
		}
		if (reset_transform)
		{
			ResetTransform();
			updated_transform = true;
		}
		if (updated_transform || owner->first_update)
		{
			UpdateTransformValues();
			if (owner->HasMesh())
			{
				owner->GetCMesh()->UpdateBoundingBox(trans_matrix_g);
				
			}
			if (owner->HasChilds())
				UpdateBBChilds(owner);
			if (owner->HasCam())
				owner->GetCCamera()->Update();

			owner->first_update = false;
		}
		
	}

}
void ComponentTransform::UpdateBBChilds(GameObject* parent)
{
	for (std::vector<GameObject*>::iterator it = parent->childs_list.begin(); it != parent->childs_list.end(); it++)
	{
		if((*it)->HasMesh())
			(*it)->GetCMesh()->UpdateBoundingBox((*it)->comp_transform->trans_matrix_g);
		//recursive call childs of childs
		if ((*it)->HasChilds())
			UpdateBBChilds((*it));
	}
}
void ComponentTransform::SetLocalPos(const float3 & new_pos)
{
	ComponentMesh* mesh;

	if (owner != nullptr)//it should never happen
	{
		mesh = (ComponentMesh*)owner->GetComponent(ComponentType::MESH);
	}
	else
		CONSOLE_LOG_WARNING("Component Transform has no owner");

	transform.pos = new_pos;
	trans_matrix_l = float4x4::FromTRS(transform.pos, transform.rot, transform.scale);
	updated_transform = true;

}

void ComponentTransform::SetGlobalPos(const float3 & new_pos)
{
	ComponentMesh* mesh;

	if (owner != nullptr)//it should never happen
	{
		mesh = (ComponentMesh*)owner->GetComponent(ComponentType::MESH);
	}
	else
		CONSOLE_LOG_WARNING("Component Transform has no owner");

	transform.pos = new_pos;
	trans_matrix_g = trans_matrix_g * float4x4::FromTRS(transform.pos, transform.rot, transform.scale);
	updated_transform = true;
}

void ComponentTransform::SetTransform(float3 pos, Quat rot,float3 scale)
{
	this->transform.pos = pos;
	this->transform.rot = rot;
	this->transform.scale = scale;
	

	trans_matrix_l = float4x4::FromTRS(pos, rot, scale);
	trans_matrix_g = float4x4::identity;
	trans_matrix_g = trans_matrix_l;
}

Transform ComponentTransform::GetTransform() const
{
	return transform;
}

void ComponentTransform::ResetTransform()
{
	transform.pos = float3::zero;
	transform.rot = Quat::identity;
	transform.scale = float3::one;
	transform.rot_euler = float3::zero;



	CalculateLocalMatrix();
}

void ComponentTransform::CalculateLocalMatrix()
{
	//reset
	trans_matrix_l = float4x4::identity;
	//scale
	trans_matrix_l = trans_matrix_l * trans_matrix_l.Scale(transform.scale);
	//roation
	trans_matrix_l = trans_matrix_l * transform.rot;
	//translation
	trans_matrix_l.SetTranslatePart(transform.pos);

	CalculateGlobalMatrix();
}

float4x4 ComponentTransform::GetGlobalMatrix()
{
	return trans_matrix_g;
}

float4x4 ComponentTransform::GetLocalMatrix()
{
	return trans_matrix_l;
}

void ComponentTransform::CalculateGlobalMatrix()
{
	//reset
	trans_matrix_g = float4x4::identity;
	GameObject* parent = owner->parent;
	//calculate me
	if (parent!=nullptr)
	{
		ComponentTransform* parent_transform = parent->comp_transform;
		trans_matrix_g = parent_transform->trans_matrix_g * trans_matrix_l;
	}
	else
	{
		trans_matrix_g =  trans_matrix_l;

	}
	//calculate my childs
	if (owner->HasChilds())
		owner->RecursiveUpdateTransformChilds();
	
	updated_transform = true;
}
