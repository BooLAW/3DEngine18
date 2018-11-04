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
	if (App->GetTermination(path) == "fbx" || App->GetTermination(path) == "FBX")
	{
		CONSOLE_LOG_INFO("FBX dropped");
		if (unique_fbx_path != path)
		{
			std::string dir_name;
			std::string final_dir_name;
			dir_name.append("Assets/Models/");
			dir_name.append(App->GetFileName(path));
			final_dir_name = dir_name.substr(0, dir_name.length()-4);
			CreateDirectory(final_dir_name.c_str(), NULL);


			App->input->file_droped = true;
			mesh_loader->LoadMesh(path,final_dir_name.c_str());
			App->scene_intro->has_meshes = true;
			unique_fbx_path = path;
		}
		else
		{
			CONSOLE_LOG_WARNING("%s was already loaded", App->GetFileName(path).c_str());
		}
		
	}
	else if (App->GetTermination(path) == "png" || App->GetTermination(path) == "PNG" || App->GetTermination(path) == "dds" || App->GetTermination(path) == "DDS" && App->scene_intro->has_meshes)
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

				comp_mat_aux->SetOwner(App->scene_intro->go_list[i]);
				App->scene_intro->go_list[i]->PushComponent((Component*)comp_aux->comp_material);
			}
				
			unique_material_path = path;
			tex_name_file = App->GetFileName(path).c_str();

		}
		else
		{
			CONSOLE_LOG_WARNING("Texture: %s was already loaded", App->GetFileName(path).c_str());
		}
		
	}
	else if (App->GetTermination(path) == "json")
	{
		App->scene_intro->ClearScene();
		App->scene_intro->LoadScene(path);
		if (unique_scene_path != path)
		{
	
			
			unique_scene_path = path;
		}
		//else
		//{
		//	CONSOLE_LOG_WARNING("Scene: %s was already loaded", App->GetFileName(path).c_str());
		//}

	}
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
