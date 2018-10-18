#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "MathGeoLib/MathGeoLib.h"

class Camera {
public:
	Camera();
	~Camera();
	//FRUSTUM OPERATIONS

	bool IsCulling()const;
	void SetCulling(bool culling);
	Frustum GetFrustum()const;

	void SetPosition(const float3& new_pos);
	void SetFOV(const float& new_fov);
	void SetFarPlane(const float& new_fp);
	void SetNearPlane(const float& new_np);
	void SetAspectRatio(const float& new_ar);

	float GetFOV()const;
	float GetVerticalFOV()const;
	float GetHorizontalFOV()const;
	float GetFarPlane()const;
	float GetNearPlane()const;
	float GetAspectRatio()const;


	//MOVEMENT
		//TODO
	
private:

	Frustum frustum;
	float	aspect_ratio = 0.0f;
	float   vertical_fov = 0.0f;

	bool	is_culling = true;
};
#endif