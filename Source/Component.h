#ifndef __COMPONENT_H__
#define __COMPONENT_H__

enum ComponentType{TANSFORM,MESH,MATERIAL,CAMERA};
class GameObject;
class Component
{
public:
	Component();
	Component(ComponentType t);

	virtual void DrawInfo();

	void Enable();
	void Disable();
	void SetOwner(GameObject* new_owner);
	bool IsActive()const;
	GameObject* GetOwner()const;
	ComponentType GetType()const;

	virtual ~Component();
public:
	ComponentType type;
	GameObject* owner;
	bool active = true;

};
#endif
