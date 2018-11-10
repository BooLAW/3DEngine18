#ifndef _APPLICATION_H__
#define _APPLICATION_H__

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleSceneImGui.h"
#include "ModuleAudio.h"
#include "LoadManager.h"
#include "Profiler.h"



class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleScene* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModuleSceneGui* imgui;
	ModuleAudio* audio;
	LoadManager* loading_manager;

	char readBuf[10000];
	char loadBuf[10000];

	static char organization_name[128];
	static char title_name[128];

	Profiler profiler;
	float	dt;
	float	sleeping_time = 0;
	float	mms_fps;

private:

	Timer	ms_timer;
	int		fps = 0;
	float	tick_interval = 0;

	bool	isSaving = false;

	std::list<Module*> list_modules;
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	void OpenWebPage(const char* url );
	void BroadcastEvent(SDL_Event & event);
	Module* GetModule(int index);
	void DebugDrawBB();

	bool Save();
	bool Load();
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


	
	void isVsyncActive();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

};
#endif