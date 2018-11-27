#include "RigidBody.h"



RigidBody::RigidBody()
{


}


RigidBody::~RigidBody()
{
}



void RigidBody::SetPosition(float x, float y, float z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
	
	


}

void RigidBody::SetRotation(float x, float y, float z)
{
	rot = Quat::FromEulerXYZ(x, y, z);
	rot_euler = { x,y,z };
}

void RigidBody::SetRotation(Quat quat)
{
	rot = quat;
	rot_euler = quat.ToEulerXYZ();
}

void RigidBody::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}
