#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Mesh.h"

class Material;
class Component;
enum ComponentType;
class ComponentTransform;
class ComponentMesh;
class ComponentCamera;
class Camera;
class GameObject
{
public:
	//CORE
	GameObject();
	~GameObject();

	void Draw();
	void DrawBB();
	void DrawNormals();
	//AABB
	AABB GetBB();
	void ResizeBB(GameObject* bb_owner);

	//COMPONENTS
	Component* GetComponent(ComponentType type);
	void PushComponent(Component* new_component);
	void RemoveComponent(ComponentType type);
	bool HasMesh()const;
	bool HasTex()const;
	bool HasCam()const;
	bool HasChilds()const;
	//flags interaction
	void SetActive(bool active);
	bool IsActive()const;
	void SetStatic(bool is_static);
	bool IsStatic()const;
	bool IsRoot()const;
	bool IsSelected()const;
	void SetSelected(bool selected);
	//Parent
	GameObject* GetParent()const;
	void SetParent(GameObject* new_parent);
	void SetParent(std::vector<GameObject*> go_list,UINT32 uid);
	void SetParentUID(UINT32 id);
	void AddChild(GameObject* new_child);
	void AddChild(std::vector<GameObject*> go_list, UINT32 uid);
	void SetChild(GameObject* child);
	GameObject* GetChild(int id);
	void DeleteChild(GameObject* child);
	int GetNumChilds()const;

	void SetUID(UINT32 id);
	//Mesh functionalities
	void ActivateBB();
	Mesh* GetMesh();
	ComponentMesh* GetCMesh();
	ComponentCamera* GetCCamera();
	Camera* GetCamera();
	Material* GetMaterial();
	void RecursiveUpdateTransformChilds();
	void RecalculateBoundingBox(GameObject* child);
	void RecursiveRecalculateBoundingBox(float4x4 transform, GameObject* go);
	//Info
	const char* GetName()const;
	void SetName(const char* name);

	void ClearRelations();
	UINT32 GetUID()const;
public:
	GameObject* parent;
	std::string name = "GO_NAME";
	std::vector<GameObject*> childs_list;
	std::vector<Component*> components_list;
	ComponentTransform* comp_transform;
	uint num_meshes;
	UINT32 uid;
	UINT32 parent_uid;
	//flags
	bool active = true;
	bool static_go = false;
	bool selected = false;
	bool root_go = false;
	bool draw_normals = false;
	bool first_update = true;

	


};

#endif