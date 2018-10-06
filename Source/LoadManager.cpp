#include "LoadManager.h"
#include "Assimp.h"
#include "Module.h"
#include "Globals.h"
#include "Application.h"
#include "Profiler.h"


LoadManager::LoadManager()
{
}


LoadManager::~LoadManager()
{
}

void LoadManager::Load(const char * path)
{
	if(App->GetTermination(path) == "fbx" || App->GetTermination(path) == "FBX")
		mesh_loader->LoadMesh(path);
}

bool LoadManager::Start()
{
	CONSOLE_LOG("---LOAD MANAGER START ---");
	App->profiler.SaveInitData("LoadManager");
	struct aiLogStream stream;

	std::string tex_path;
	tex_path.append("Assets/Textures/Baker_house.png");
	//GLuint final_tex = material_loader->LoadPNG(tex_path);
	

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
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


