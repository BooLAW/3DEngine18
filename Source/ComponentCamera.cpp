#include "ComponentCamera.h"
#include "TextureMSAA.h"
#include "Camera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "Transform.h"

ComponentCamera::ComponentCamera()
{
	type = CAMERA;
	SetName("Component Camera");
	
	cam = new Camera();
	
}

bool ComponentCamera::Start()
{
	ComponentTransform* trans = (ComponentTransform*)owner->GetComponent(ComponentType::TANSFORM);
	cam->SetPosition(trans->transform.pos);


	return true;
}

bool ComponentCamera::Update()
{
	//debug draw for the frustum
	return false;
}


ComponentCamera::~ComponentCamera()
{
}

Camera * ComponentCamera::GetCamera()
{
	return cam;
}


