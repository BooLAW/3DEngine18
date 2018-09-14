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
	void showSphereCreator();
	void showCubeCreator();

public:
	//flags
	bool show_test_window = false;
	bool show_console = false;
	bool show_sphere_creator = false;
	bool show_square_creator = false;

	//Geometry Creator
	int rad_aux = 0;
	int x_aux = 0, y_aux = 0, z_aux = 0;
	int x_aux2 = 0, y_aux2 = 0, z_aux2 = 0;
	vec pos_aux;
	vec min_pos_aux;
	vec max_pos_aux;
};
