#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Mesh.h"
class Component;
enum ComponentType;
class ComponentTransform;
class GameObject
{
public:
	//CORE
	GameObject();
	~GameObject();
	void Draw();
	//AABB
	AABB GetBB();
	void ResizeBB(GameObject* bb_owner);

	//COMPONENTS
	Component* GetComponent(ComponentType type);
	void PushComponent(Component* new_component);
	void RemoveComponent(ComponentType type);

	bool HasMesh()const;
	bool HasTex()const;

	//flags interaction
	void SetActive(bool active);
	bool IsActive()const;
	void SetStatic(bool is_static);
	bool IsStatic()const;
	bool IsRoot()const;
	//Parent
	GameObject* GetParent()const;
	void SetParent(GameObject* new_parent);
	void AddChild(GameObject* new_child);
	void SetChild(GameObject* child);
	GameObject* GetChild(int id);

	//Mesh functionalities
	void ActivateBB();
	Mesh* GetMesh();
	
	//Info
	const char* GetName()const;
	void SetName(const char* name);


public:
	GameObject* parent;
	std::string name = "GO_NAME";
	std::vector<GameObject*> childs_list;
	std::vector<Component*> components_list;
	ComponentTransform* transform;
	uint num_meshes;
	//flags
	bool active = true;
	bool static_go = false;
	bool root_go = false;

	


};

#endif