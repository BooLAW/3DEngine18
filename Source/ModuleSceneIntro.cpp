#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#include "ImGui/imgui_impl_sdl.h"
#include <stdlib.h>
#include <time.h>
#include "ModuleJSON.h"



#define RADIUS 44

ModuleSceneIntro::ModuleSceneIntro( bool start_enabled) : Module( start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	CONSOLE_LOG("Loading Intro assets");
	bool ret = true;



	//App->json->CreateNewJSON("testconfig.json");

	json_file file1("testconfig.json");
	const char* name_output = file1.ReadString("name");
	int age_output = file1.ReadInt("age");

	//for testing -> //App->json->print_commits_info("torvalds", "linux");



	//-----------------------------
	srand(time(NULL));
	
	App->camera->Move(float3(1.0f, 5.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	CONSOLE_LOG("Unloading Intro scene");
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Render the Plane
	/* PAU
	Plane p({0, 1, 0}, 0);
	p..axis = true;
	p.Render(); */

	return UPDATE_CONTINUE;
}



void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

int ModuleSceneIntro::RandInt(int min, int max)
{
	LCG aux;
	int ret;
	ret = aux.Int(min, max);
	return ret;
}

float ModuleSceneIntro::RandFloat(float min, float max)
{

	LCG aux;
	float ret;
	ret = aux.Float(min, max);
	return ret;
}



