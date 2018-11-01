#include "Component.h"
#include "GameObject.h"


Component::Component()
{
}

Component::Component(ComponentType t)
{
	type = t;
}

void Component::DrawInspectorInfo()
{
}

bool Component::Update()
{
	return true;
}

void Component::Enable()
{
	active = true;
}

void Component::Disable()
{
	active = false;
}

void Component::SetOwner(GameObject * new_owner)
{
	owner = new_owner;
}

bool Component::IsActive() const
{
	return active;
}

void Component::SetActive(bool active)
{
	this->active = active;
}

GameObject * Component::GetOwner() const
{
	return owner;
}

ComponentType Component::GetType() const
{
	return type;
}

ComponentMaterial * Component::GetComponentMaterial() const
{

	return comp_material;
}

void Component::SetType(ComponentType type)
{
	this->type = type;
}

const char * Component::GetName()
{
	return name.c_str();
}

void Component::SetName(const char * _name)
{
	name = _name;
}

Component::~Component()
{
}
