#ifndef __COMPONENT_TRANSFORM__
#define __COMPONENT_TRANSFORM__
#include "Component.h"
#include "Transform.h"
class GameObject;
class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* owner);
	virtual ~ComponentTransform();
	
	bool Update()override;

	void UpdateTransformValues();
	void DrawInspectorInfo();

	void SetLocalPos(const float3& new_pos);
	void SetGlobalPos(const float3& new_pos);

	void SetTransform(float3 pos, Quat rot, float3 scale);
	Transform* GetTransform()const;
	void ResetTransform();

	void CalculateLocalMatrix();
	void SetLocalMatrix(const float4x4 new_local);
	float4x4 GetGlobalMatrix();
	float4x4 GetLocalMatrix();
	void CalculateGlobalMatrix();
	void UpdateBBChilds(GameObject* parent);
	void UpdateTransformFromGuizmo(const float4x4 new_global);
	void LogicUpdate();


public:
	Transform* transform;

	float4x4 trans_matrix_l;
	float4x4 trans_matrix_g;	

	bool updated_transform;
	bool updated_outside;
};

#endif