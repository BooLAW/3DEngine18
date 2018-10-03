#include "Component.h"
#include "GameObject.h"


Component::Component()
{
}

Component::Component(ComponentType t)
{
	type = t;
}

void Component::DrawInfo()
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

GameObject * Component::GetOwner() const
{
	return owner;
}

ComponentType Component::GetType() const
{
	return type;
}


Component::~Component()
{
}
