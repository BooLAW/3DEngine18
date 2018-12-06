#ifndef __COMPONENT_H__
#define __COMPONENT_H__
#include "Globals.h"

enum ComponentType{TRANSFORM,MESH,MATERIAL,CAMERA,RIGIDBODY};
class GameObject;
class ComponentMaterial;
class ComponentPhysBody;
class Component
{
public:
	Component();
	Component(ComponentType t);

	virtual void DrawInspectorInfo();
	virtual bool Update();
	virtual void Enable();
	virtual void Disable();
	void SetOwner(GameObject* new_owner);
	bool IsActive()const;
	void SetActive(bool active);
	GameObject* GetOwner()const;
	bool HasOwner()const;
	ComponentType GetType()const;
	ComponentMaterial* GetComponentMaterial()const;
	void SetType(ComponentType type);
	const char* GetName();
	void SetName(const char* _name);
	virtual ~Component();

public:
	ComponentType type;
	ComponentMaterial* comp_material;
	ComponentPhysBody* comp_rigidbody;
	GameObject* owner;
	std::string name;
	bool active = true;

};
#endif
