#pragma once
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
	void UpdatePos();
public:
	Camera * cam;

};
