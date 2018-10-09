#ifndef __LOAD_MANAGER_H__
#define __LOAD_MANAGER_H__
#include "Module.h"
#include "MeshLoader.h"
#include "MaterialLoader.h"



enum resourceType
{
	RESOURCE_MESH,
	RESORCE_MATERIAL,
	RESOURCE_NULL
};
class Resource
{
public:
	Resource(const char* name, resourceType type);
	~Resource();

	//virtual void Init();

	int id;

private:
	std::string name;
	resourceType type;
};
class LoadManager : public Module
{
public:
	LoadManager();
	 ~LoadManager();

	void Load(const char* path);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();
public:
	std::list<Resource> resources;
	MeshLoader* mesh_loader;
	MaterialLoader* material_loader;
	std::string unique_material_path;
	std::string unique_fbx_path;


};

#endif