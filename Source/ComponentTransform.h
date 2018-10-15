#ifndef __COMPONENT_TRANSFORM__
#define __COMPONENT_TRANSFORM__
#include "Component.h"
#include "Transform.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* owner);
	virtual ~ComponentTransform();
	bool Update()override;

	void SetLocalPos(const float3& new_pos);
	void SetGlobalPos(const float3& new_pos);

	void SetTransform(float3 pos, Quat rot, float3 scale);
	Transform GetTransform()const;

public:
	Transform transform;

	float4x4 trans_matrix_l;
	float4x4 trans_matrix_g;

	bool updated_transform;
};

#endif