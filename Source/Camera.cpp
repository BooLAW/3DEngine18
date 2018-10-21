#include "Camera.h"
#include "Globals.h"
Camera::Camera()
{
	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);

	frustum.SetPos(float3(0, 1, -1));
	frustum.SetFront(float3(0, 0, 1));
	frustum.SetUp(float3(0, 1, 0));
	aspect_ratio = 0;

	
}

Camera::~Camera()
{
}

void Camera::SetPosition(const float3 & new_pos)
{
	frustum.SetPos(new_pos);
}

void Camera::SetFront(const float3 & front)
{
	frustum.SetFront(front);
}

void Camera::SetFOV(const float & new_fov)
{
	if (new_fov <= 0)
	{
		CONSOLE_LOG_WARNING("Can't set a fov of negative value : %d", new_fov);
	}
	else
	{
		vertical_fov = new_fov * DEGTORAD;
		frustum.SetVerticalFovAndAspectRatio(DEGTORAD*new_fov, aspect_ratio);
	}
}

void Camera::SetFarPlane(const float & new_fp)
{
	frustum.SetViewPlaneDistances(frustum.nearPlaneDistance, new_fp);
}

void Camera::SetNearPlane(const float & new_np)
{
	frustum.SetViewPlaneDistances(new_np, frustum.farPlaneDistance);
}

void Camera::SetAspectRatio(const float & new_ar)
{
	if (new_ar <= 0)
	{
		CONSOLE_LOG_WARNING("Can't set a fov of negative value : %d", new_ar);
	}
	else
	{
		frustum.SetVerticalFovAndAspectRatio(vertical_fov, new_ar);
	}
}

float Camera::GetVerticalFOV() const
{
	return frustum.VerticalFov() * RADTODEG;
}

float Camera::GetHorizontalFOV() const
{
	return frustum.HorizontalFov() * RADTODEG;
}

float Camera::GetFarPlane() const
{
	return frustum.FarPlaneDistance();
}

float Camera::GetNearPlane() const
{
	return frustum.NearPlaneDistance();
}

float Camera::GetAspectRatio() const
{
	return aspect_ratio;
}


bool Camera::IsCulling() const
{
	return is_culling == true;
}

void Camera::SetCulling(bool culling)
{
	is_culling = culling;
}

Frustum Camera::GetFrustum()const
{
	return frustum;
}


