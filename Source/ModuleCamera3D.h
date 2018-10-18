#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "MathGeoLib\MathGeoLib.h"

class TextureMSAA; 
class PanelScene;
class Camera;
class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void DrawModuleConfig()override;

	void CameraMovement(float dt);

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &speed);
	void WheelMove(const vec3 &mouse_speed,int direction);
	void HandleMouse();
	void Orbit();
	float* GetViewMatrix();

	void SetSpeed(float new_speed); 
	float GetSpeed() const;

	void SetMouseSensitivity(float new1_sensitivity);
	float GetMouseSensitivity() const;
	TextureMSAA* SceneMSAA();

	void LockCamera(); 
	void UnlockCamera(); 
	bool IsLocked(); 

	bool Save(Document& config_w, FileWriteStream& os);
	bool Load(Document* config_r);
	void AdaptCamera(AABB bounding_box);
	void MoveCam(const vec3 &speed);
	bool MouseOverScene(PanelScene* Scene);

private:

	void CalculateViewMatrix();
	float speed_base = 4.0f;
	float wheel_speed_base = 4.0f;
	float mouse_sensitivity = 0.5f;
	bool locked = false; 

public:
	
	vec3 X, Y, Z, Position, Reference;

private:
	//active camera when playing game
	Camera * game_camera = nullptr;
	//camera of the scene in the editor
	Camera * editor_camera = nullptr;
	
	std::vector<Camera*> cams_list;

	mat4x4 ViewMatrix, ViewMatrixInverse;
	TextureMSAA* viewport_texture; 
};