#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ModuleSceneImGui.h"
#include "Globals.h"
#include "Camera.h"
#include "Application.h"
#include "ComponentPhysBody.h"

ComponentTransform::ComponentTransform(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component Transform");
	transform = new Transform();
	transform->pos = float3(0.0f, 0.0f, 0.0f);
	transform->rot = Quat::identity;
	transform->rot_euler = float3(0.0f, 0.0f, 0.0f);
	transform->scale = float3(1.0f, 1.0f, 1.0f);

	CalculateLocalMatrix();

	trans_matrix_g = float4x4::identity;
	updated_transform = true;
	updated_outside = false;
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
	trans_matrix_l = float4x4::FromTRS(transform->pos, transform->rot, transform->scale);

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

	float pos[3] = { transform->pos.x,transform->pos.y,transform->pos.z };
	float rot[3] = { transform->rot.ToEulerXYZ().x * RADTODEG, transform->rot.ToEulerXYZ().y * RADTODEG, transform->rot.ToEulerXYZ().z *RADTODEG };
	float scale[3] = { transform->scale.x,transform->scale.y,transform->scale.z };
	transform->rot_euler = (float3)rot;

	if (ImGui::Button("Identity Reset"))
		reset_transform = !reset_transform;

	if (ImGui::DragFloat3("Position##transform", pos, 0.1f, -INFINITY, INFINITY))
	{
		App->imgui->guizmo_operation = TRANSLATE;
	}
	if (ImGui::DragFloat3("Rotation##transform", rot, 0.1f, -360, 360))
	{
		App->imgui->guizmo_operation = ROTATE;
	}
	if (ImGui::DragFloat3("Scale##transform", scale, 0.1f, 1, INFINITY))
	{
		App->imgui->guizmo_operation = SCALE;
	}

	
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
		if (!transform->rot_euler.Equals(rot[0],rot[1],rot[2]))
		{
			transform->SetRotation(aux);
			updated_transform = true;
		}
		if (scale[0] != transform->scale.x || scale[1] != transform->scale.y || scale[2] != transform->scale.z)
		{
			transform->SetScale(scale[0], scale[1], scale[2]);
			updated_transform = true;
		}
		if (reset_transform)
		{
			ResetTransform();
			updated_transform = true;
		}
		if (updated_outside)
		{
			UpdateTransformValues();
			updated_outside = false;
		}
		if (updated_transform || owner->GetFirstUpdate())
		{
			UpdateTransformValues();
			if (owner->HasRigidBody())
			{
				owner->GetRigidBody()->owner_trans_updated = true;
			}
			if (owner->HasMesh())
			{
				owner->GetCMesh()->UpdateBoundingBox(trans_matrix_g);
				
			}
			if (owner->HasChilds())
				UpdateBBChilds(owner);
			if (owner->HasCam())
				owner->GetCCamera()->Update();

			owner->SetFirstUpdate(false);
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

	transform->pos = new_pos;
	trans_matrix_l = float4x4::FromTRS(transform->pos, transform->rot, transform->scale);
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

	transform->pos = new_pos;
	trans_matrix_g = trans_matrix_g * float4x4::FromTRS(transform->pos, transform->rot, transform->scale);
	updated_transform = true;
}

void ComponentTransform::SetTransform(float3 pos, Quat rot,float3 scale)
{
	this->transform->pos = pos;
	this->transform->rot = rot;
	this->transform->scale = scale;
	
	float4x4 aux = float4x4::identity;
	aux = aux * aux.Scale(transform->scale);
	aux = aux * transform->rot;
	aux.SetTranslatePart(transform->pos);

	trans_matrix_l = aux;
	CalculateGlobalMatrix();
}

Transform* ComponentTransform::GetTransform() const
{
	return transform;
}

void ComponentTransform::ResetTransform()
{
	transform->pos = float3::zero;
	transform->rot = Quat::identity;
	transform->scale = float3::one;
	transform->rot_euler = float3::zero;



	CalculateLocalMatrix();
}

void ComponentTransform::CalculateLocalMatrix()
{
	//reset
	trans_matrix_l = float4x4::identity;
	//scale
	trans_matrix_l = trans_matrix_l * trans_matrix_l.Scale(transform->scale);
	//roation
	trans_matrix_l = trans_matrix_l * transform->rot;
	//translation
	trans_matrix_l.SetTranslatePart(transform->pos);

	CalculateGlobalMatrix();
}

void ComponentTransform::SetLocalMatrix(const float4x4 new_local)
{
	trans_matrix_l = new_local;

	float3 pos; Quat rot; float3 scale;
	new_local.Decompose(pos, rot, scale);
	
	SetTransform(pos, rot, scale);

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

void ComponentTransform::UpdateTransformFromGuizmo(const float4x4 new_global)
{
	trans_matrix_g = new_global;
	if (owner->GetParent() != nullptr)
		trans_matrix_l = owner->GetParent()->comp_transform->trans_matrix_g.Inverted() * trans_matrix_g;
	else
		trans_matrix_l = trans_matrix_g;

	float3 pos;Quat rot; float3 scale;

	trans_matrix_l.Decompose(pos, rot, scale);

	SetTransform(pos, rot, scale);

}
