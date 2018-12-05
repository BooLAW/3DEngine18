#ifndef __PhysBody_H__
#define __PhysBody_H__

#include "Module.h"

class btRigidBody;
class Module;

// =================================================
struct PhysBody
{
	friend class ModulePhysics3D;
public:
	PhysBody(btRigidBody* body);
	PhysBody();
	~PhysBody();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void SetAsSensor(bool is_sensor);
	bool IsSensor() const;
	btRigidBody* GetRigidBody();
	void SetMass(const uint new_mass);
	uint GetMass()const;
private:
	btRigidBody * body = nullptr;
	bool is_sensor = false;
	uint mass = 0;
	bool use_gravity = false;

public:
	std::vector<Module*> collision_listeners;
};

#endif // __PhysBody3D_H__