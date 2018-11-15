#include "LoadManager.h"
#include "Assimp.h"
#include "Module.h"
#include "Globals.h"
#include "Application.h"
#include "Profiler.h"
#include "ComponentMaterial.h"


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
			std::string dir_name;
			std::string main_dir_name;
			std::string secondary_dir_name;

			dir_name = GetFolderNameFBX(path);
			secondary_dir_name = GetSecondaryFolderNameFBX(dir_name.c_str());
			main_dir_name = GetMainFolderNameFBX(dir_name.c_str());
			

			CreateDirectory(main_dir_name.c_str(), NULL); //Create Library Folder
			CreateDirectory(secondary_dir_name.c_str(), NULL); //Create Models Folder
			CreateDirectory(dir_name.c_str(), NULL); //Create Mesh Folder

			App->scene_intro->folder_path = dir_name;
			
			
			App->input->file_droped = true;

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

Resource * LoadManager::CreateNewResource(resourceType type, INT32 force_uid)
{

	return nullptr;
}

bool LoadManager::Start()
{
	CONSOLE_LOG_INFO("---LOAD MANAGER START ---");
	App->profiler.SaveInitData("LoadManager");
	struct aiLogStream stream;

	
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

Resource::Resource(const char * name, resourceType type)
{
	this->type = type;
	this->name = name;

}

Resource::~Resource()
{
}

void AssimpLog(const char * message, char * user)
{
	CONSOLE_LOG_INFO("%s", message);
}
INT32 LoadManager::ImportFile(const char * new_file_path, bool force)
{
	std::string termination = GetTermination(new_file_path).c_str();
	if (termination == "lw")
	{
		resourceType type;
		type = RESOURCE_MESH;
		Resource* res = CreateNewResource(type,CreateRandUID());
	}
	
	return INT32();
}
INT32 LoadManager::CreateRandUID()
{
	//Create Random UID for mesh Root
	unsigned int max_int = UINT_MAX;
	UINT32 random_int = pcg32_boundedrand_r(&App->imgui->rng, max_int) + 1000000000;
	return random_int;
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
	std::string dir_name;
	std::string final_dir_name;
	dir_name.append("Library/Models/");
	dir_name.append(App->loading_manager->GetFileName(path));
	final_dir_name = dir_name.substr(0, dir_name.length() - 4);
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