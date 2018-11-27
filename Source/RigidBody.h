#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#include "MathGeoLib/MathGeoLib.h"

class RigidBody
{
public:
	RigidBody();
	virtual ~RigidBody();
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotation(Quat quat);
	void SetScale(float x, float y, float z);
public:
	float3 pos = float3::zero;
	Quat rot = Quat::identity;
	float3 rot_euler = float3::zero;
	float3 scale = float3::one;
};

#endif