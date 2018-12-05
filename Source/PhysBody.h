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
	void ActivateGravity(const bool active);
	bool HasGravity()const;
private:
	btRigidBody * body = nullptr;
	bool is_sensor = false;
	//Unity Elements------------
	//Mass
	uint mass = 0;
	//Affected by gravity?
	bool use_gravity = false;
	//Collision Detection
public:
	//Constraints
	bool const_px;
	bool const_py;
	bool const_pz;

	bool const_rx;
	bool const_ry;
	bool const_rz;

	//--------------------------

public:
	std::vector<Module*> collision_listeners;
};

#endif // __PhysBody3D_H__