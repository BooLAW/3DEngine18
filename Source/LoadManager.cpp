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
		CONSOLE_LOG("FBX dropped",INFO_LOG);
		if (unique_fbx_path != path)
		{
			App->scene_intro->ClearScene();
			App->input->file_droped = true;
			mesh_loader->LoadMesh(path);
			App->scene_intro->has_meshes = true;
			unique_fbx_path = path;
		}
		else
		{
			CONSOLE_LOG("%s was already loaded",WARN_LOG, App->GetFileName(path).c_str());
		}
		
	}
	else if (App->GetTermination(path) == "png" || App->GetTermination(path) == "PNG" || App->GetTermination(path) == "dds" || App->GetTermination(path) == "DDS" && App->scene_intro->has_meshes)
	{
		CONSOLE_LOG("Texture Dropped",INFO_LOG);	
		if (unique_material_path != path)
		{
			App->input->tex_droped = true;

			ComponentMaterial* aux = material_loader->LoadPNG(path);
			int auxi = App->scene_intro->go_list.size();
			for (int i = 1; i < auxi; i++)
				App->scene_intro->go_list[i]->PushComponent((Component*)aux);
			unique_material_path = path;
			tex_name_file = App->GetFileName(path).c_str();

		}
		else
		{
			CONSOLE_LOG("Texture: %s was already loaded",WARN_LOG, App->GetFileName(path).c_str());
		}
		
	}
}

bool LoadManager::Start()
{
	CONSOLE_LOG("---LOAD MANAGER START ---",INFO_LOG);
	App->profiler.SaveInitData("LoadManager");
	struct aiLogStream stream;

	
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	material_loader->Start();
	aiAttachLogStream(&stream);
	return false;
}

update_status LoadManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool LoadManager::CleanUp()
{
	CONSOLE_LOG("---LOAD MANAGER CLEANUP ---",INFO_LOG);
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


