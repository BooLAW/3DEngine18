#pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"


struct PhysBody3D;

class ModuleSceneGui : public Module
{
public:
	ModuleSceneGui(Application* app, bool start_enabled = true);
	~ModuleSceneGui();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	int CreateMainMenu();
	void showTestWindow();
public:
	bool show_test_window = false;
	bool show_console = false;
};
