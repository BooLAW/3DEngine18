#pragma once
#include "Module.h"
#include "Globals.h"
#include "Panel.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Quadtree.h"

struct PhysBody3D;
class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	int RandInt(int min, int max);
	float RandFloat(float min, float max);
	void Log(const std::string text);
	GameObject* CreateNewGameObject();
	void DrawGameObjects();
	void ClearScene();
	bool HasObjects();
	//Octree
	void AddToOctree(GameObject* go);
	void CollectOctreeIntersections(std::list<Mesh*>& item_elements,AABB* bounding_box);
	//UI
	void DrawInspector();
	void DrawHierarchy();
	void DrawChilds(GameObject* parent);
	void ResetSelected();
	GameObject* GetSelected();
	void SaveScene(std::vector<GameObject*> go_list);


public:
	//Scene GameObjects(Now only meshes)
	std::vector<GameObject*> go_list;
	std::vector<GameObject*> go_to_draw;
	std::vector<Mesh*> octree_meshes;
	GameObject* scene_root = nullptr;
	GameObject* imported_go = nullptr;
	Octree octree;
	bool draw_octree = false;
	bool has_meshes = false;
	std::string fbx_name;

};
