#ifndef __ComponentCamera__H__
#define __ComponentCamera__H__
#include "Component.h"
class Camera;
class TextureMSAA;
class ComponentCamera :
	public Component
{
public:
	ComponentCamera();
	bool Start();
	bool Update()override;
	virtual ~ComponentCamera();
	Camera* GetCamera();
	void UpdateTransform();
public:
	Camera * cam;
	bool game_camera;
	bool user_rotate = false;

};
#endif
