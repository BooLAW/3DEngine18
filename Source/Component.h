#ifndef __COMPONENT_H__
#define __COMPONENT_H__
#include "Globals.h"

enum ComponentType{TRANSFORM,MESH,MATERIAL,CAMERA,RIGIDBODY,COLLIDERSPHERE,COLLIDERCUBE,CONTROLLER};
class GameObject;
class ComponentMaterial;
class ComponentRigidBody;
class ComponentPlayerController;
class ComponentColliderSphere;
class ComponentColliderCube;
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
	ComponentPlayerController* GetController()const;
	void SetType(ComponentType type);
	const char* GetName();
	void SetName(const char* _name);
	virtual ~Component();

public:
	ComponentType type;
	ComponentMaterial* comp_material;
	ComponentRigidBody* comp_rigidbody;
	ComponentColliderSphere* comp_collider_sphere;
	ComponentColliderCube* comp_collider_cube;
	ComponentPlayerController* comp_controller;
	GameObject* owner;
	std::string name;
	bool active = true;

};
#endif
