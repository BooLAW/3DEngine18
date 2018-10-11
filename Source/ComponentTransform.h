#pragma once
#include "Component.h"
#include "Transform.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* owner);
	virtual ~ComponentTransform();

	void SetTransform(float3 pos, Quat rot, float3 scale);
	Transform GetTransform()const;

public:
	Transform transform;
	//Transform global_trans;
};

