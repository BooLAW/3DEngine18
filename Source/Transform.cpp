#include "Transform.h"



Transform::Transform()
{


}


Transform::~Transform()
{
}



void Transform::SetPosition(float x, float y, float z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
	
	


}

void Transform::SetRotation(float x, float y, float z)
{
	rot = Quat::FromEulerXYZ(x, y, z);
	rot_euler = { x,y,z };
}

void Transform::SetRotation(Quat quat)
{
	rot = quat;
	rot_euler = quat.ToEulerXYZ();
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}
