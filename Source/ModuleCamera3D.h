#ifndef __ModuleCamera3D__H__
#define __ModuleCamera3D__H__
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib\MathGeoLib.h"

class TextureMSAA; 
class PanelScene;
class Camera;
class GameObject;
class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();
	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void DrawModuleConfig()override;

	void CameraMovement(float dt);

	void Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference = false);
	void LookAt(const float3 &Spot);
	void Move(const float &speed);
	void WheelMove(const float &mouse_speed,int direction);
	void HandleMouse(const float dt);
	void Orbit(const float dt);
	float* GetViewMatrix();

	void SetSpeed(float new_speed); 
	float GetSpeed() const;

	void SetMouseSensitivity(float new1_sensitivity);
	float GetMouseSensitivity() const;

	void LockCamera(); 
	void UnlockCamera(); 
	bool IsLocked(); 

	bool Save(Document& config_w, FileWriteStream& os);
	bool Load(Document* config_r);
	void AdaptCamera(AABB bounding_box,float3 transformpos);
	void AdaptCamera(float3 transformpos);
	void MoveCam(const float3 &speed);
	bool MouseOverScene(PanelScene* Scene);
	Camera* GetCurrentCam()const;
	void StartEditorCamera();
private:
	float speed_base = 4.0f;
	float wheel_speed_base = 4.0f;
	float mouse_sensitivity = 0.5f;
	bool locked = false; 


public:
	//active camera when playing game
	//camera of the scene in the editor
	GameObject* editor_camera_gameobject = nullptr;
	Camera* editor_cam = nullptr;
	bool draw_frustum = false;
	std::vector<Camera*> cams_list;

};
#endif