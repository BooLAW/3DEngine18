#include "Collider.h"
#include "ModulePhysics3D.h"
#include "Application.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// =================================================
Collider::Collider(btRigidBody* body) : body(body)
{
	body->setUserPointer(this);
}

Collider::Collider()
{
}

// ---------------------------------------------------------
Collider::~Collider()
{
	delete body;
}

// ---------------------------------------------------------
void Collider::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// ---------------------------------------------------------
void Collider::GetTransform(float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

void Collider::SetMass(const uint new_mass)
{
	mass = new_mass;
}

uint Collider::GetMass() const
{
	return mass;
}

void Collider::ActivateGravity(const bool active)
{
	use_gravity = active;
	use_gravity ? body->setGravity({ 0,App->physics->GetGravity(),0 }) : body->setGravity({ 0,0,0 });
}

bool Collider::HasGravity()const
{
	return use_gravity;
}



// ---------------------------------------------------------
void Collider::SetTransform(float* matrix)
{
	if (body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// ---------------------------------------------------------
void Collider::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}


// ---------------------------------------------------------
void Collider::SetAsSensor(bool is_sensor)
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
bool Collider::IsSensor() const
{
	return is_sensor;
}






