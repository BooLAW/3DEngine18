#include "ComponentTransform.h"


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

void ComponentTransform::SetTransform(float3 pos, Quat rot,float3 scale)
{
	this->transform.pos = pos;
	this->transform.rot = rot;
	this->transform.scale = scale;

}
