#include "ComponentConstraint.h"
#include "GameObject.h"



ComponentConstraint::ComponentConstraint()
{
}
ComponentConstraint::ComponentConstraint(GameObject* owner)
{
	this->SetOwner(owner);
	this->SetActive(true);
	SetName("Component Constraint");

	type = ComponentType::CONSTRAINT;
}


ComponentConstraint::~ComponentConstraint()
{
}

ConstraintType ComponentConstraint::GetConsType() const
{
	return cons_type;
}
