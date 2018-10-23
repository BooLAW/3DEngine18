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
	
}

bool ComponentCamera::Start()
{
	ComponentTransform* trans = (ComponentTransform*)owner->GetComponent(ComponentType::TANSFORM);
	UpdatePos();


	return true;
}

bool ComponentCamera::Update()
{
	owner->transform->SetGlobalPos(cam->GetFrustum().pos);
	return false;
}


ComponentCamera::~ComponentCamera()
{
}

Camera * ComponentCamera::GetCamera()
{
	return cam;
}

void ComponentCamera::UpdatePos()
{
	cam->GetFrustum().pos = GetOwner()->transform->trans_matrix_g.TranslatePart();
	cam->GetFrustum().front = GetOwner()->transform->trans_matrix_g.WorldZ().Normalized();
	cam->GetFrustum().up = GetOwner()->transform->trans_matrix_g.WorldY().Normalized();
}


