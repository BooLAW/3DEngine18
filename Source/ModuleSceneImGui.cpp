#include "Globals.h"
#include "Application.h"
#include "ModuleSceneImGui.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"


ModuleSceneGui::ModuleSceneGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneGui::~ModuleSceneGui()
{}

// Load assets
bool ModuleSceneGui::Start()
{
	LOG("Loading gui assets");
	bool ret = true;
	ImGui_ImplSdl_Init(App->window->window);



	return ret;
}

// Load assets
bool ModuleSceneGui::CleanUp()
{
	LOG("Unloading gui scene");
	ImGui_ImplSdl_Shutdown();

	return true;
}

update_status ModuleSceneGui::PreUpdate(float dt)
{
	ImGui_ImplSdl_NewFrame(App->window->window);
	return UPDATE_CONTINUE;
}
// Update
update_status ModuleSceneGui::Update(float dt)
{
	switch (CreateMainMenu())
	{
		case UPDATE_STOP:
		{
			return UPDATE_STOP;
		}
	}

	//ImGui::ShowTestWindow();
	return UPDATE_CONTINUE;
}

update_status ModuleSceneGui::PostUpdate(float dt)
{
	ImGui::Render();
	return UPDATE_CONTINUE;
}


void ModuleSceneGui::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

int ModuleSceneGui::CreateMainMenu()
{
	//Creates the example window
	// Menu
	static bool show_app_main_menu_bar = true;
	if (show_app_main_menu_bar)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit", "Alt + F4")) { return UPDATE_STOP; }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Demo")) { ImGui::EndMenu(); }
			ImGui::EndMainMenuBar();
		}
	}
}
