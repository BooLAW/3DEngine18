#ifndef __COMPONENT_RIGIDBODY__
#define __COMPONENT_RIGIDBODY__
#include "Component.h"

class GameObject;
class PhysBody;
class ComponentPhysBody : public Component
{
public:
	ComponentPhysBody(GameObject* owner);
	virtual ~ComponentPhysBody();
	
	bool Update()override;

	void DrawInspectorInfo();
	void UpdateTransform();
	bool HasMoved();
	bool IsBulletStatic();
public:
	PhysBody* physbody;
	bool bullet_trans_updated;
	bool owner_trans_updated;
};

#endif