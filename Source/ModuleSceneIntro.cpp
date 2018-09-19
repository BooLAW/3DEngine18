#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "PanelConsole.h"
#include "PanelConfiguration.h"
#include "ImGui\imgui_impl_sdl.h"

#include <stdlib.h>
#include <time.h>


#define RADIUS 44

ModuleSceneIntro::ModuleSceneIntro( bool start_enabled) : Module( start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	//Add Panels HERE
	console = new PanelConsole();
	configuration = new PanelConfiguration();
	panels.push_back(console);
	panels.push_back(configuration);

	//-----------------------------
	srand(time(NULL));
	
	App->camera->Move(float3(1.0f, 5.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

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

	//Blit all the Panels
	for (std::vector<Panel*>::iterator item = panels.begin(); item != panels.end(); ++item)
	{
		if ((*item)->IsActive())
		{
			
			ImVec2 pos;
			ImVec2 size;
			pos.x = (*item)->pos_x;
			pos.y = (*item)->pos_y;
			size.x = (*item)->width;
			size.y = (*item)->height;

			ImGui::SetNextWindowPos(pos, ImGuiSetCond_Always);
			ImGui::SetNextWindowSize(size, ImGuiSetCond_Always);
			(*item)->Draw();
		}
	}
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

void ModuleSceneIntro::Log(char * text)
{
	if (console != nullptr)
		console->LogToConsole(text);
}


