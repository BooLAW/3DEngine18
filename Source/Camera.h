#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "MathGeoLib/MathGeoLib.h"
#define FRUSTUM_PLANES 6

class TextureMSAA;
class GameObject;
class Camera {
public:
	Camera();
	~Camera();
	//FRUSTUM OPERATIONS

	bool IsCulling()const;
	void SetCulling(bool culling);
	Frustum GetFrustum();

	void SetFront(const float3& front);
	void SetUp(const float3& up);

	void SetFOV(const float& new_fov);
	void SetFarPlane(const float& new_fp);
	void SetNearPlane(const float& new_np);
	void SetAspectRatio(const float& new_ar);

	float GetVerticalFOV()const;
	float GetHorizontalFOV()const;
	float GetFarPlane()const;
	float GetNearPlane()const;
	float GetAspectRatio()const;

	float* GetViewMatrix()const;
	float4x4 GetViewMatrix4x4()const;

	float* GetProjMatrix()const;
	float4x4 GetProjMatrix4x4()const;

	void UpdateProjectionMatrix();

	//void CalculateViewMatrix();

	//MOVEMENT
	void UpdatePosition(float3 newpos);
	void Look(const float3 &Position, const float3 &Reference, bool Pivoting);
	void LookAt(const float3& at);
	void HandleMouse(const float dt);
	void RotateGameCamera(const float sensitivity);
	void CreateNewFrustum();
	void DrawFrustum();
	TextureMSAA* SceneMSAA();
	bool IsGameObjectInFrustum(AABB& bb, float3 translation);
	TextureMSAA* viewport_texture;
	Frustum frustum;
	void ActivateFrustum(bool var);
	bool IsFrustumActive()const;

private:

	bool draw_frustum = false;
	float	aspect_ratio = 0.0f;
	float3 frustum_vertices[8];
	bool	is_culling = true;
};
#endif