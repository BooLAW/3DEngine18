#include "ComponentPhysBody.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ModuleSceneImGui.h"
#include "Globals.h"
#include "Camera.h"
#include "Application.h"
#include "PhysBody.h"
ComponentPhysBody::ComponentPhysBody(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component PhysBody");

	physbody = new PhysBody();
	type = ComponentType::RIGIDBODY;


}

ComponentPhysBody::~ComponentPhysBody()
{
}

bool ComponentPhysBody::Update()
{
	
	return false;
}



void ComponentPhysBody::DrawInspectorInfo()
{
	int mass = physbody->GetMass();
	if (ImGui::InputInt("Mass", &mass))
		physbody->SetMass(mass);

}

