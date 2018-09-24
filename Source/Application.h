#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleSceneImGui.h"
#include "ModuleJSON.h"


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModuleSceneGui* imgui;
	ModuleJSON* json;

private:

	Timer	ms_timer;
	float	dt;
	int		fps = 0;
	float	maxfps = 0;
	float	sleeping_time = 0;
	
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
private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};