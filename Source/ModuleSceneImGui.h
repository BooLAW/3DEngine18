#pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"

struct PhysBody3D;

class ModuleSceneGui : public Module
{
public:
	ModuleSceneGui(bool start_enabled = true);
	~ModuleSceneGui();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	int CreateMainMenu();
	void showTestWindow();
	void showConfigurationWindow();
	void showConsoleWindow();
	void showSphereCreator();
	void showRandomNumberGenerator();
	void showCubeCreator();

public:
	//flags
	bool show_test_window = false;
	bool show_console = false;
	bool show_sphere_creator = false;
	bool show_cube_creator = false;
	bool show_random_number_generator = false;

	//Geometry Creator
	int rad_aux = 0;
	int x_aux = 0, y_aux = 0, z_aux = 0;
	int x_aux2 = 0, y_aux2 = 0, z_aux2 = 0;
	int aux1 = 0, aux2 = 1;
	int random_int = 0;
	float random_float = 0.0f;
	
	vec pos_aux;
	vec min_pos_aux;
	vec max_pos_aux;
};
