#include "ComponentTransform.h"


ComponentTransform::ComponentTransform(GameObject * owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	type = ComponentType::TANSFORM;

}

ComponentTransform::~ComponentTransform()
{
}
