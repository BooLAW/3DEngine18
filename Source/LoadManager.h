#ifndef __LOAD_MANAGER_H__
#define __LOAD_MANAGER_H__
#include "Module.h"
#include "MeshLoader.h"
#include "MaterialLoader.h"
#include "PanelConsole.h"



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
	std::string unique_scene_path;

	//RandUINT
	INT32 GetRandUID();

	//File path string
	std::string GetTermination(const char* path);

	std::string EraseTerination(const char* path);

	std::string GetFileName(const char* path);

	//Returns the path of the folder containing the meshes ex: Library/Models/BakerHouse
	std::string GetFolderNameFBX(const char* path);

	//Retunrs the path of the main folder ex: Library/
	std::string GetMainFolderNameFBX(const char* path);

	//Returns the path of the secondary folder ex: Library/Models/
	std::string GetSecondaryFolderNameFBX(const char* path);

	std::string GetFolderNameLW(const char* path);

	std::string tex_name_file;
};
void AssimpLog(const char* message, char* user);

#endif