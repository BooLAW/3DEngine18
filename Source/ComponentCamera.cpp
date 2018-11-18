#include "ComponentCamera.h"
#include "TextureMSAA.h"
#include "Camera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "Transform.h"
#include "MathGeoLib/MathGeoLib.h"
#include "GameObject.h"

ComponentCamera::ComponentCamera()
{
	type = CAMERA;
	SetName("Component Camera");
	
	cam = new Camera();
	game_camera = true;
}

bool ComponentCamera::Start()
{
	ComponentTransform* trans = (ComponentTransform*)owner->GetComponent(ComponentType::TRANSFORM);
	UpdateTransform();

	return true;
}

bool ComponentCamera::Update()
{
	if (game_camera)
		UpdateTransform();
	
	cam->CreateNewFrustum();
	
	return false;
}


ComponentCamera::~ComponentCamera()
{
}

Camera * ComponentCamera::GetCamera()
{
	return cam;
}

void ComponentCamera::UpdateTransform()
{
	ComponentTransform* trans = (ComponentTransform*)owner->GetComponent(ComponentType::TRANSFORM);
	//ComponentTransform* trans = owner->comp_transform;
	Transform my_owner_trans = trans->transform;
	cam->frustum.pos = my_owner_trans.pos;

	float3 eul = my_owner_trans.rot_euler;


	cam->frustum.front = *(float3*)&(float4x4::RotateAxisAngle({ 1,0,0 }, eul.x * DEGTORAD) * float4({ 0,0,-1 }, 1.0f));
	cam->frustum.up = *(float3*)&(float4x4::RotateAxisAngle({ 1,0,0 }, eul.x * DEGTORAD) * float4({ 0,1,0 }, 1.0f));
	cam->frustum.front = *(float3*)&(float4x4::RotateAxisAngle({ 0,1,0 }, eul.y * DEGTORAD) * float4(cam->frustum.front, 1.0f));
	cam->frustum.up = *(float3*)&(float4x4::RotateAxisAngle({ 0,1,0 }, eul.y * DEGTORAD) * float4(cam->frustum.up, 1.0f));
	cam->frustum.front = *(float3*)&(float4x4::RotateAxisAngle({ 0,0,1 }, eul.z * DEGTORAD) * float4(cam->frustum.front, 1.0f));
	cam->frustum.up = *(float3*)&(float4x4::RotateAxisAngle({ 0,0,1 }, eul.z * DEGTORAD) * float4(cam->frustum.up, 1.0f));

}


