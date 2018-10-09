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
