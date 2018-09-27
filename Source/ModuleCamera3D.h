#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "MathGeoLib\MathGeoLib.h"

class TextureMSAA; 

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void DrawModuleConfig()override;

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

	void SetSpeed(float new_speed); 
	float GetSpeed() const;

	void SetMouseSensitivity(float new_sensitivity);
	float GetMouseSensitivity() const;

	TextureMSAA* GetViewportTexture(); 

	void LockCamera(); 
	void UnlockCamera(); 
	bool IsLocked(); 

	bool Save(Document& config_w, FileWriteStream& os);
	bool Load(Document* config_r);

private:

	void CalculateViewMatrix();
	float speed_base = 4.0f;
	float mouse_sensitivity = 0.5f;
	bool locked; 

public:
	
	vec3 X, Y, Z, Position, Reference;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
	TextureMSAA* viewport_texture; 
};