#ifndef __LOAD_MANAGER_H__
#define __LOAD_MANAGER_H__
#include "Module.h"

enum resourceType
{
	RESOURCE_MESH,
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

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
public:
		std::list<Resource> resources;
};

#endif