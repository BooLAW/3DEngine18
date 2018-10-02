#include "LoadManager.h"



LoadManager::LoadManager()
{
}


LoadManager::~LoadManager()
{
}

bool LoadManager::Start()
{
	return false;
}

update_status LoadManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool LoadManager::CleanUp()
{
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


