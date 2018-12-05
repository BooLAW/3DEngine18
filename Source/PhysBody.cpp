#include "PhysBody.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// =================================================
PhysBody::PhysBody(btRigidBody* body) : body(body)
{
	body->setUserPointer(this);
}

PhysBody::PhysBody()
{
}

// ---------------------------------------------------------
PhysBody::~PhysBody()
{
	delete body;
}

// ---------------------------------------------------------
void PhysBody::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// ---------------------------------------------------------
void PhysBody::GetTransform(float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

btRigidBody * PhysBody::GetRigidBody()
{
	return body;
}

void PhysBody::SetMass(const uint new_mass)
{
	mass = new_mass;
}

uint PhysBody::GetMass() const
{
	return mass;
}

void PhysBody::ActivateGravity(const bool active)
{
	use_gravity = active;
}

bool PhysBody::HasGravity()const
{
	return use_gravity;
}

// ---------------------------------------------------------
void PhysBody::SetTransform(const float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// ---------------------------------------------------------
void PhysBody::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}


// ---------------------------------------------------------
void PhysBody::SetAsSensor(bool is_sensor)
{
	if (this->is_sensor != is_sensor)
	{
		this->is_sensor = is_sensor;
		if (is_sensor == true)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		else
			body->setCollisionFlags(body->getCollisionFlags() &~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}

// ---------------------------------------------------------
bool PhysBody::IsSensor() const
{
	return is_sensor;
}






