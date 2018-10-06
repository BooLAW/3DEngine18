#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"

class Component;
enum ComponentType;
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
public:
	GameObject* parent;
	std::string name = "GO_NAME";
	std::vector<GameObject*> childs_list;
	std::vector<Component*> components_list;
	//flags
	bool active = true;
	bool static_go = false;
	bool root_go = false;

	


};

#endif