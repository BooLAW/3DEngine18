#ifndef __COMPONENT_RIGIDBODY__
#define __COMPONENT_RIGIDBODY__
#include "Component.h"

class GameObject;
class PhysBody;
class ComponentCollider : public Component
{
public:
	ComponentCollider(GameObject* owner);
	virtual ~ComponentCollider();
	
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