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
	virtual bool Update();
	virtual void Enable();
	virtual void Disable();
	void SetOwner(GameObject* new_owner);
	bool IsActive()const;
	void SetActive(bool active);
	GameObject* GetOwner()const;
	ComponentType GetType()const;
	void SetType(ComponentType type);

	virtual ~Component();
public:
	ComponentType type;
	GameObject* owner;
	bool active = true;

};
#endif
