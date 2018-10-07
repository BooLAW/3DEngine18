#include "Application.h"
#include "ImGui/imgui.h"
#include "Component.h"
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
	loading_manager = new LoadManager();
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
	AddModule(loading_manager);
	
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
	CONSOLE_LOG("Application Start --------------");
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		ret = (*item)->Start();
	}
	Load();



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
	if (App->imgui->isVsyncActive == false)
	{
		maxfps = 1000.0f / (float)App->imgui->fps_slider;
		sleeping_time = maxfps - dt - 3;
		Sleep(sleeping_time);
	}
	if (App->imgui->want_to_save == true)
	{
		Save();
	}
	if (App->imgui->want_to_load == true)
	{
		Load();
	}
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

void Application::DebugDraw()
{
	//change to active GO
	for (int i = 0; i < scene_intro->go_list.size(); i++)
		scene_intro->go_list[i]->ActivateBB();

	/*for (int i = 0; i < scene_intro->go_list.size(); i++)
		scene_intro->go_list[i]->show_normal = !scene_intro->go_list[i]->show_normal;
*/
}

bool Application::Save()
{
	bool ret = true;

	FILE* fp = fopen("testconfig.json", "wb"); // non-Windows use "w"
	char writeBuffer[10000];
	Document testconfig_w;
	testconfig_w.Parse(readBuf);
	testconfig_w.SetObject();
	FileWriteStream os(fp, readBuf, sizeof(readBuf));

	Document::AllocatorType& allocator = testconfig_w.GetAllocator();

	Value app(kObjectType);


	app.AddMember("engine_name", TITLE, allocator);
	app.AddMember("organization", ORGANIZATION, allocator);
	app.AddMember("max_fps", App->imgui->fps_slider, allocator);
	testconfig_w.AddMember("app", app, allocator);


	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		ret = (*item)->Save(testconfig_w,os);
	}
	Writer<FileWriteStream> writer(os);
	testconfig_w.Accept(writer);
	fclose(fp);
	App->imgui->want_to_save = false;
	return ret;
}

bool Application::Load()
{
	bool ret = true;
	//const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
	FILE* fp = fopen("testconfig.json", "rb"); // non-Windows use "r"

	Document testconfig_r;
	FileReadStream is(fp, loadBuf, sizeof(loadBuf));
	if (is.Peek() == 0)
	{
		App->imgui->want_to_save = true;

	}
	else
	{
		testconfig_r.ParseStream(is);
		testconfig_r.IsObject();


		maxfps = testconfig_r["app"]["max_fps"].GetInt();
		App->imgui->fps_slider = maxfps;

		std::string title;
		title.append(testconfig_r["app"]["engine_name"].GetString());
		title.append(" - ");
		title.append(testconfig_r["app"]["organization"].GetString());
		for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
		{
			ret = (*item)->Load(&testconfig_r);
		}
	}

	fclose(fp);
	App->imgui->want_to_load = false;
	return ret;
}

std::string Application::GetTermination(const char * path)
{
	std::string ret;

	std::string fn = path;
	ret = fn.substr(fn.find_last_of(".") + 1);

	return ret;
}

std::string Application::GetFileName(const char * path)
{
	std::string full_path_str(path);
	std::string ret;

	//DeleteEndBars(full_path_str);

	if (full_path_str.find('\\') != 0)
	{
		uint cut = full_path_str.find_last_of('\\');

	
		ret = full_path_str.substr(cut + 1, full_path_str.length() - cut);
	}

	return ret;


}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}
