#include "LoadManager.h"
#include "Assimp.h"
#include "Module.h"
#include "Globals.h"
#include "Application.h"
#include "Profiler.h"
#include "ComponentMaterial.h"


LoadManager::LoadManager()
{
}


LoadManager::~LoadManager()
{
}

void LoadManager::Load(const char * path)
{
	if (App->GetTermination(path) == "fbx" || App->GetTermination(path) == "FBX")
	{
		CONSOLE_LOG("FBX dropped");
		App->scene_intro->ClearScene();
		mesh_loader->LoadMesh(path);
		App->scene_intro->has_meshes = true;
	
	}
	else if (App->GetTermination(path) == "png" || App->GetTermination(path) == "PNG" && App->scene_intro->has_meshes)
	{
		CONSOLE_LOG("Texture Dropped");	
		ComponentMaterial* aux = material_loader->LoadPNG(path);
		int auxi = App->scene_intro->go_list.size();
		for(int i = 1;i < auxi; i++)
			App->scene_intro->go_list[i]->PushComponent((Component*)aux);
		//App->scene_intro->go_list[2]->PushComponent((Component*)aux);

	}
}

bool LoadManager::Start()
{
	CONSOLE_LOG("---LOAD MANAGER START ---");
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
	CONSOLE_LOG("---LOAD MANAGER CLEANUP ---");
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


