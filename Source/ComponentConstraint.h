#pragma once
#include "Component.h"
enum ConstraintType {
	P2P,
	NONE
};
class GameObject;
class ComponentConstraint : public Component
{
public:
	ComponentConstraint();
	ComponentConstraint(GameObject* owner);

	virtual ~ComponentConstraint();
	ConstraintType GetConsType()const;
private:
	ConstraintType cons_type;

};

