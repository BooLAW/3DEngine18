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
	TextureMSAA* SceneMSAA();
public:
	Camera * cam;
	TextureMSAA* viewport_texture;

};

