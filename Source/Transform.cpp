#include "Transform.h"



Transform::Transform()
{
	pos = float3(0.0f, 0.0f, 0.0f);
	rot = Quat::identity;
	scale = float3(1.0f, 1.0f, 1.0f);

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
}

void Transform::SetRotation(Quat quat)
{
	rot = quat;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}
