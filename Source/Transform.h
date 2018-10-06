#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#include "MathGeoLib/MathGeoLib.h"

class Transform
{
public:
	Transform();
	virtual ~Transform();
public:
	float3 pos = float3::zero;
	Quat rot = Quat::identity;
	float3 scale = float3::one;
};

#endif