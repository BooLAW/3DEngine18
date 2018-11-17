#include "LoadManager.h"
#include "Assimp.h"
#include "Module.h"
#include "Globals.h"
#include "Application.h"
#include "Profiler.h"
#include "ComponentMaterial.h"

#include <direct.h>
#define GetCurrentDir _getcwd

#include "dirent.h"
#include <sys/types.h>


LoadManager::LoadManager()
{
	unique_material_path = "";
}


LoadManager::~LoadManager()
{
	
}

void LoadManager::Load(const char * path)
{
	if (GetTermination(path) == "fbx" || GetTermination(path) == "FBX")
	{
		CONSOLE_LOG_INFO("FBX dropped");
		if (unique_fbx_path != path)
		{
			CreateFolders(path);
			
			App->input->file_droped = true;

			//ImportFile(path);

			mesh_loader->LoadMesh(path);

			App->scene_intro->has_meshes = true;
			unique_fbx_path = path;
		}
		else
		{
			CONSOLE_LOG_WARNING("%s was already loaded", GetFileName(path).c_str());
		}
		
	}
	else if (GetTermination(path) == "png" || GetTermination(path) == "PNG" || GetTermination(path) == "dds" || GetTermination(path) == "DDS" && App->scene_intro->has_meshes)
	{
		CONSOLE_LOG_INFO("Texture Dropped");	
		if (unique_material_path != path)
		{
			App->input->tex_droped = true;
			Component* comp_aux = new Component(MATERIAL);
			ComponentMaterial* comp_mat_aux = material_loader->LoadPNG(path);			
			comp_aux->comp_material = comp_mat_aux;
			int auxi = App->scene_intro->go_list.size();
			for (int i = 1; i < auxi; i++)
			{
				if (App->scene_intro->go_list[i])
				{
					comp_mat_aux->SetOwner(App->scene_intro->go_list[i]);
					App->scene_intro->go_list[i]->PushComponent((Component*)comp_aux->comp_material);
				}				
			}
				
			unique_material_path = path;
			tex_name_file = GetFileName(path).c_str();

		}
		else
		{
			CONSOLE_LOG_WARNING("Texture: %s was already loaded", GetFileName(path).c_str());
		}
		
	}
	else if (GetTermination(path) == "json")
	{
		if (unique_scene_path != path)
		{			
			
			App->scene_intro->LoadScene(path);
			unique_scene_path = path;

		}
		else
		{
			CONSOLE_LOG_WARNING("Scene: %s was already loaded", path);
		}

	}
	else if (GetTermination(path) == "lw")
	{

		std::string sub_beg;
		std::string old_path(path);
		std::string new_path;
		uint cut = old_path.find("Models");
		cut = cut + 7;
		uint cut2 = old_path.find_last_of("\\");
		cut2 = cut2 - cut;
		sub_beg = old_path.substr(cut , cut2);
		new_path.append("Assets/Models/");
		new_path.append(sub_beg.c_str());

		App->scene_intro->folder_path = new_path;
		
		App->loading_manager->mesh_loader->LoadMesh(path);
	}
}

void LoadManager::LoadAssetsFolder()
{	
	CONSOLE_LOG_INFO("---COPY ASSETS FOLDER INTO LIBRARY ---");

	//Init empty buffer
	char working_dir_path[FILENAME_MAX];

	//Get working folder from visual
	GetCurrentDir(working_dir_path, sizeof(working_dir_path));

	//Adding it to an std::string
	std::string working_dir(working_dir_path);

	//Check it's folders and put them in a list.
	CheckFiles(working_dir_path);


	for (std::vector<std::string>::iterator it = all_files_paths.begin(); it != all_files_paths.end(); it++)
	{
		std::string library("Library");
		std::size_t found_it = (*it).find(library);
		if (found_it != std::string::npos)
		{
			llibrary_paths.push_back(*it);
		}
		else
		{
			assets_paths.push_back(*it);
		}		
	}

	//Comparing both folders
	for (std::vector<std::string>::iterator it = assets_paths.begin(); it != assets_paths.end(); it++)
	{
		if (GetTermination((*it).c_str()) == "fbx"||
			GetTermination((*it).c_str()) == "FBX" )
		{			
			load_paths.push_back((*it));
		}
	}
	
}

void LoadManager::CheckFiles(std::string folder_path)
{
	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(folder_path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			std::string path(folder_path);
			path.append("\\");
			path.append(ent->d_name);

			GetTermination(path.c_str());

			//Get all the files with this format
			if (GetTermination(path.c_str()) == "fbx" ||
				GetTermination(path.c_str()) == "FBX" ||
				GetTermination(path.c_str()) == "png" ||
				GetTermination(path.c_str()) == "dds" ||
				GetTermination(path.c_str()) == "mtl" ||
				GetTermination(path.c_str()) == "tga" ||
				GetTermination(path.c_str()) == "ogg")
			{
				all_files_paths.push_back(path);
			}

			//Get Folder
			uint found_it2 = path.find_first_of(".");
			if (found_it2 == MAXUINT)
			{
				CheckFiles(path);
			}
		}
		closedir(dir);
	}
}

bool LoadManager::Start()
{
	CONSOLE_LOG_INFO("---LOAD MANAGER START ---");
	App->profiler.SaveInitData("LoadManager");
	struct aiLogStream stream;

	LoadAssetsFolder();
	
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	material_loader->Start();
	stream.callback = AssimpLog;
	aiAttachLogStream(&stream);
	return false;
}
update_status LoadManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool LoadManager::CleanUp()
{
	CONSOLE_LOG_INFO("---LOAD MANAGER CLEANUP ---");
	aiDetachAllLogStreams();
	return false;
}

Resource::Resource(const char * name, resourceType type, INT32 uid)
{
	this->type = type;
	this->name.append(name);
	this->id = uid;
}

Resource::~Resource()
{
}

std::string Resource::GetName()
{

	return this->name;
}

resourceType Resource::GetType()
{
	return this->type;
}

void AssimpLog(const char * message, char * user)
{
	CONSOLE_LOG_INFO("%s", message);
}
UINT32 LoadManager::Find(const char * path)
{
	for (std::map<UINT32, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		const char* name = it->second->GetName().c_str();		
		if (strcmp(name, path))
		{
			return it->first;
		}		
	}
	return NULL;
}
UINT32 LoadManager::ImportFile(const char* new_file_path, bool force)
{
	std::string termination = GetTermination(new_file_path).c_str();
	if (termination == "lw")
	{
		resourceType type;
		type = RESOURCE_MESH;
		UINT32 res_uid = CreateRandUID();
		
		Resource res = Resource(new_file_path, type,res_uid);
		
		resources.insert(std::pair<UINT32, Resource*>(res_uid, &res));		
		return res_uid;
	}
	return NULL;	
}
UINT32 LoadManager::CreateRandUID()
{
	//Create Random UID for mesh Root
	unsigned int max_int = UINT_MAX;
	UINT32 random_int = pcg32_boundedrand_r(&App->imgui->rng, max_int) + 1000000000;
	return random_int;
}

Resource * LoadManager::Get(UINT32 uid)
{	
	for (std::map<UINT32, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->first == uid)
		{
			return it->second;
		}
	}
	return nullptr;
}

std::string LoadManager::GetTermination(const char * path)
{
	std::string ret;

	std::string fn = path;
	ret = fn.substr(fn.find_last_of(".") + 1);

	return ret;
}

std::string LoadManager::EraseTerination(const char * path)
{
	std::string ret;

	std::string fn = path;
	ret = fn.substr(0, fn.find_last_of("."));
	return ret;
}

std::string LoadManager::GetFileName(const char * path)
{
	std::string full_path_str(path);
	std::string ret;

	//DeleteEndBars(full_path_str);
	std::size_t found_it = full_path_str.find('\\');
	if (found_it != UINT_MAX)
	{
		uint cut = full_path_str.find_last_of('\\');
		ret = full_path_str.substr(cut + 1, full_path_str.length() - cut);
	}
	else if (full_path_str.find('//') >= 1)
	{
		uint cut = full_path_str.find_last_of('//');
		ret = full_path_str.substr(cut + 1, full_path_str.length() - cut);
	}
	found_it = full_path_str.find('/');
	if (found_it != UINT_MAX)
	{
		uint cut = full_path_str.find_last_of('/');
		ret = full_path_str.substr(cut + 1, full_path_str.length() - cut);
	}
	return ret;
}

std::string LoadManager::GetFolderNameFBX(const char * path)
{
	std::string input_path(path);
	std::string dir_name;
	std::string final_dir_name;

	//Loding From Assets Folder
	std::size_t found_it = input_path.find("Assets");
	if (found_it != UINT_MAX)
	{
		dir_name.append("Library/Models/");
		dir_name.append(App->loading_manager->GetFileName(path));
		final_dir_name = dir_name.substr(0, dir_name.length() - 4);
	}
	else//Loading From Library Models folder (scene1.json)
	{
		dir_name.append("Library/Textures/");

		uint cut1 = input_path.find("Models/") + 7;
		uint cut2 = input_path.find_last_of("/");

		dir_name.append(input_path.substr(cut1, cut2 - cut1));

		final_dir_name = dir_name;
	}

	

	return final_dir_name;
}

std::string LoadManager::GetMainFolderNameFBX(const char * path)
{
	std::string input_path(path);
	std::string dir_name;
	std::string final_dir_name;

	uint cut = input_path.find_first_of("/") + 1;


	final_dir_name = input_path.substr(0, cut);
	return final_dir_name;
}

std::string LoadManager::GetSecondaryFolderNameFBX(const char * path)
{
	std::string input_path(path);
	std::string dir_name;
	std::string final_dir_name;

	uint cut = input_path.find_last_of("/");

	final_dir_name = input_path.substr(0, cut);
	return final_dir_name;
}

std::string LoadManager::GetFolderNameLW(const char * path)
{
	std::string input_path(path);
	std::string dir_name;
	std::string final_dir_name;
	dir_name.append("Library/Models/");
	uint cut = input_path.find("Models/") + 7;
	uint cut2 = input_path.find_last_of("/");

	final_dir_name = input_path.substr(cut, cut2 - cut);
	dir_name.append(final_dir_name.c_str());
	return dir_name;
}

void LoadManager::CreateFolders(const char * path)
{
	std::string dir_name;
	std::string main_dir_name;
	std::string secondary_dir_name;

	dir_name = App->loading_manager->GetFolderNameFBX(path);
	secondary_dir_name = App->loading_manager->GetSecondaryFolderNameFBX(dir_name.c_str());
	main_dir_name = App->loading_manager->GetMainFolderNameFBX(dir_name.c_str());

	CreateDirectory(main_dir_name.c_str(), NULL); //Create Library Folder
	CreateDirectory(secondary_dir_name.c_str(), NULL); //Create SubFolders inside Library
	CreateDirectory(dir_name.c_str(), NULL); //Create Folder with then name of the mesh

	App->scene_intro->folder_path = dir_name;
}

void LoadManager::ImportTextures(const char * path,const char* full_path)
{
	//Check if it has textures to load from the assets/textures folder
	bool found_it = false;
	std::string texture_name(path);
	if (texture_name.size() > 0)
	{
		for (int i = 0; App->loading_manager->assets_paths.size() > i; i++)
		{
			if (App->loading_manager->GetFileName(App->loading_manager->assets_paths[i].c_str()) == texture_name.c_str())
			{
				found_it = true;
			}
		}
	}

	if (found_it)
	{
		//Create Materials Folder inside Library and FBX name folder inside Materials
		std::string input_path = App->loading_manager->GetFileName(full_path);
		std::string tex_folder_path;
		tex_folder_path.append("Library/Textures");
		CreateDirectory(tex_folder_path.c_str(), NULL);

		//Creates BakerHouse Folder inside Materials folder
		tex_folder_path.append("/");
		tex_folder_path.append(App->loading_manager->EraseTerination(input_path.c_str()));
		CreateDirectory(tex_folder_path.c_str(), NULL);

		std::string tex_path("Assets/Textures/");
		tex_path.append(texture_name.c_str());

		//Storing the texture inside the Library
		std::string lib_tex_path("Library/Textures/");
		lib_tex_path.append(App->loading_manager->EraseTerination(input_path.c_str()));
		lib_tex_path.append("/");
		lib_tex_path.append(texture_name.c_str());
		CopyFile(tex_path.c_str(), lib_tex_path.c_str(), NULL);

	}
}
