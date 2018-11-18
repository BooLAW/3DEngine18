#ifndef __LOAD_MANAGER_H__
#define __LOAD_MANAGER_H__
#include "Module.h"
#include "MeshLoader.h"
#include "MaterialLoader.h"
#include "PanelConsole.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "Mesh.h"
#include <map>


enum resourceType
{
	RESOURCE_MESH,
	RESOURCE_MATERIAL,
	RESOURCE_NULL
};
class Resource
{
public:
	Resource(const char* path, resourceType type, INT32 uid);
	Resource* CreateNewResource(const char* path, resourceType type);
	~Resource();
	std::string GetName();
	Component* GetComponent();
	resourceType GetType();
	
	//virtual void Init();

	INT32 id;

private:
	std::string path;
	resourceType type;
	Component* comp;

};
class LoadManager : public Module
{
public:
	LoadManager();
	 ~LoadManager();

	void Load(const char* path);

	void LoadAssetsFolder();
	void StoreFilesPath(std::string folder_path);

	//UIDs methods
	UINT32 Find(const char* path);
	UINT32 ImportFile(const char* new_file_path, bool force = false);
	UINT32 GenerateNewUID();

	//Resources
	Resource* Get(UINT32 uid);
	

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

	//Create the directories needed checking the paths
	void CreateFolders(const char* path);

	//Import Textures
	void ImportTextures(const char* path, const char* full_path);


	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	std::vector<std::string> all_files_paths;
	std::vector<std::string> llibrary_paths;
	std::vector<std::string> assets_paths;
	std::vector<std::string> load_paths;

	std::map<UINT32, Resource*> resources;

	MeshLoader* mesh_loader;
	MaterialLoader* material_loader;

	std::string unique_material_path;
	std::string unique_fbx_path;
	std::string unique_scene_path;


	std::string tex_name_file;

};
void AssimpLog(const char* message, char* user);

#endif