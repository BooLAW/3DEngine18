#pragma once
#include "Component.h"
#include "Module.h"
#include "Application.h"


class GameObject;
class Material;
class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	ComponentMaterial(GameObject* owner);
	virtual ~ComponentMaterial();

	void Enable();
	void Disable();

	void InitTexture();
	void EndTexture();
public:
	Material* data = nullptr;
};

