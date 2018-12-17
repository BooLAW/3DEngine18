#ifndef __COMPONENT_RIGIDBODY__
#define __COMPONENT_RIGIDBODY__
#include "Component.h"

class GameObject;
class PhysBody;
class ComponentRigidBody : public Component
{
public:
	ComponentRigidBody(GameObject* owner);
	virtual ~ComponentRigidBody();
	
	bool Update()override;

	void DrawInspectorInfo();
	void UpdateTransform();
	bool HasMoved();
	bool IsBulletStatic();
public:
	bool bullet_trans_updated;
	bool owner_trans_updated;
	int comp_mass = 1;
	bool use_gravity = false;
};

#endif