#include "Application.h"
#include "ImGui/imgui.h"
#include "PanelConfiguration.h"

Application::Application()
{
	window = new ModuleWindow();
	input = new ModuleInput();
	audio = new ModuleAudio();
	scene_intro = new ModuleSceneIntro();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	physics = new ModulePhysics3D();
	imgui = new ModuleSceneGui();
	json = new ModuleJSON(true);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	AddModule(json);
	
	// Scenes
	AddModule(imgui);
	AddModule(scene_intro);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		delete (*item);	
	}
	
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		ret = (*item)->Init();
	}
	

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		ret = (*item)->Start();
	}
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	maxfps = 1000.0f / (float)App->imgui->fps_slider;
	sleeping_time = maxfps - dt-3;
	Sleep(sleeping_time);

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	++fps;
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for(std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		if(ret == UPDATE_CONTINUE)
			ret = (*item)->PreUpdate(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		if (ret == UPDATE_CONTINUE)
			ret = (*item)->Update(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		if (ret == UPDATE_CONTINUE)
			ret = (*item)->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		ret = (*item)->CleanUp();
	}
		
	
	return ret;
}

void Application::OpenWebPage(const char * url )
{
	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
	
}

void Application::BroadcastEvent(SDL_Event & event)
{
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++)
	{
		(*it)->BroadcastEvent(event);
	}
}

Module * Application::GetModule(int index)
{
	if (index <= list_modules.size())
	{
		int i = 0;

		for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
		{
			if (i == index)
				return (*item);

			i++;
		}
	}
	return nullptr;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}
