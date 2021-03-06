#ifndef __COMPONENT_COLLIDER_SPHERE__
#define __COMPONENT_COLLIDER_SPHERE__
#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;
class PhysBody;
class PSphere;
class ComponentColliderSphere : public Component
{
public:
	ComponentColliderSphere(GameObject* owner);
	ComponentColliderSphere(GameObject* owner,PSphere* psphere);
	virtual ~ComponentColliderSphere();
	
	bool Update()override;

	void DrawInspectorInfo();
	void UpdateTransform();
	bool HasMoved();
	bool IsBulletStatic();
public:
	bool bullet_trans_updated;
	bool owner_trans_updated;
	float center_offset[3] = { 0,0,0 };
	float radius = 1;

};

#endif