#pragma once
#include "Component.h"
#include "Transform.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* owner);
	virtual ~ComponentTransform();

public:
	Transform local_trans;
	Transform global_trans;
};

