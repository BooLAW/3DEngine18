#ifndef __ModuleScene_H__
#define __ModuleScene_H__
#include "Module.h"
#include "Globals.h"
#include "Panel.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Octree.h"

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
	void DeleteGameObject(GameObject* go_to_delete);
	void DeleteGameObjectsInList();
	bool HasObjects();
	GameObject* CreateMainCamera();
	//Octree
	void AddToOctree(GameObject* go);
	void RemoveFromOctree(GameObject * go);
	void CollectOctreeIntersections(std::list<Mesh*>& item_elements,AABB* bounding_box);

	//UI
	void DrawHierarchy();
	void DrawModuleConfig()override;
	void DrawChilds(GameObject* parent);

	//Selected
	void ResetSelected();
	GameObject* GetSelected();

	//Save&Load
	void SaveScene(std::vector<GameObject*> go_list);
	void LoadScene(const char* path);
	Value SaveGO(GameObject* go, Document::AllocatorType& allocator);
	//MousePicking
	GameObject* GetClosestGO(std::vector<GameObject*> gameobjects, LineSegment line);
	void ClickSelection(LineSegment mouse_ray);
public:
	//Scene GameObjects(Now only meshes)
	std::vector<GameObject*> go_list;	
	std::vector<GameObject*> to_delete;
	std::list<GameObject*> octree_objects;
	
	GameObject* scene_root = nullptr;
	GameObject* imported_go = nullptr;
	
	Octree octree;
	
	std::string fbx_name;
	std::string folder_path;
	
	int id_new_go = 0;
	char scenewriteBuffer[10000];

	bool draw_octree = false;
	bool has_meshes = false;


};

#endif