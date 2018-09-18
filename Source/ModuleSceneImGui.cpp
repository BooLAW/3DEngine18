#include "Globals.h"
#include "Application.h"
#include "ModuleSceneImGui.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl.h"


ModuleSceneGui::ModuleSceneGui(bool start_enabled) : Module( start_enabled)
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
	//RICARD: For a cleaner code, we decided to have a switch with the CreateMainMenu.
	switch (CreateMainMenu())
	{
		case UPDATE_STOP:
		{
			return UPDATE_STOP;
		}
	}

	return UPDATE_CONTINUE;
}

update_status ModuleSceneGui::PostUpdate(float dt)
{
	
	App->renderer3D->SetUILights();
	ImGui::Render();
	return UPDATE_CONTINUE;
}


void ModuleSceneGui::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

int ModuleSceneGui::CreateMainMenu()
{
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
			if (ImGui::BeginMenu("Tools")) 
			{
				if (ImGui::MenuItem("ImGui Demo"))
				{
					ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_Always);
					show_test_window = !show_test_window;
				}
				if (ImGui::MenuItem("Random Number Generator"))
				{
					show_random_number_generator = !show_random_number_generator;
				}
				if (ImGui::MenuItem("Sphere Creator"))
				{
					show_sphere_creator = !show_sphere_creator;
				}
				if (ImGui::MenuItem("Cube Creator"))
				{
					show_cube_creator = !show_cube_creator;
				}
				if (ImGui::MenuItem("Check Collisions"))
				{
					App->physics->GetSphereCollisions();
					App->physics->GetCubeCollisions();
				}
				ImGui::EndMenu(); 
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Console"))
				{
					App->scene_intro->panels[0]->Activate();
				}
				/*
				if (ImGui::MenuItem("Configuration"))
				{
					App->scene_intro->panels[1]->Activate();
				}*/
			
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("About..."))
			{
				ImGui::Text("Creative Worlds");
				ImGui::Text("by Pau Bonet & Josep Pi");
				ImGui::Text("Creative World is a game engine to create 3D world with realisitic physics.");
				ImGui::Spacing();				
				if (ImGui::MenuItem("Link to Repository"))
				{
					App->OpenWebPage("https://github.com/BooLAW/3DEngine18");
				}
				if (ImGui::MenuItem("License"))
				{
					App->OpenWebPage("https://github.com/BooLAW/3DEngine18/blob/master/LICENSE");
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	if (show_test_window)showTestWindow();
	if (show_sphere_creator)showSphereCreator();
	if (show_cube_creator)showCubeCreator();
	if (show_random_number_generator)showRandomNumberGenerator();
}

void ModuleSceneGui::showTestWindow() 
{
	ImGui::ShowTestWindow(&show_test_window);

}

void ModuleSceneGui::showConsoleWindow()
{

}

void ModuleSceneGui::showSphereCreator()
{
	if (ImGui::Begin("Sphere Creator"))
	{
		ImGui::InputInt("Radius", &rad_aux);
		ImGui::InputInt("Position X", &x_aux);
		ImGui::InputInt("Position Y", &y_aux);
		ImGui::InputInt("Position Z", &z_aux);
		if (ImGui::SmallButton("Create"))
		{
			pos_aux.Set(x_aux, y_aux, z_aux);
			App->physics->CreateSphere(pos_aux, rad_aux);

		}
		if (ImGui::SmallButton("Reset"))
		{
			x_aux = y_aux = z_aux = rad_aux = 0;
		}

		ImGui::End();
	}
	
}

void ModuleSceneGui::showRandomNumberGenerator()
{

	if (ImGui::Begin("Random Number Generator"))
	{
		ImGui::InputInt("Max Number ", &aux2);
		ImGui::InputInt("Min Number", &aux1);
		ImGui::Spacing();
		if (ImGui::SmallButton("Generate Random Int"))
		{
			random_int = App->scene_intro->RandInt(aux1, aux2);
		}
		ImGui::SameLine();
		ImGui::Text("%i", random_int);
		ImGui::Spacing();
		if (ImGui::SmallButton("Generate Random Float"))
		{
			random_float = App->scene_intro->RandFloat(aux1, aux2);
		}
		ImGui::SameLine();
		ImGui::Text("%f", random_float);
		ImGui::End();
	}

}

void ModuleSceneGui::showCubeCreator()
{
	if (ImGui::Begin("Cube Creator"))
	{
		ImGui::Text("minPoint");
		ImGui::InputInt("Position X", &x_aux);
		ImGui::InputInt("Position Y", &y_aux);
		ImGui::InputInt("Position Z", &z_aux);
		ImGui::Spacing();
		ImGui::Text("maxPoint");
		ImGui::InputInt("Position X2", &x_aux2);
		ImGui::InputInt("Position Y2", &y_aux2);
		ImGui::InputInt("Position Z2", &z_aux2);

		if (ImGui::SmallButton("Create"))
		{
			min_pos_aux.Set(x_aux, y_aux, z_aux);
			max_pos_aux.Set(x_aux2, y_aux2, z_aux2);
			App->physics->CreateCube(pos_aux, max_pos_aux);

		}
		if (ImGui::SmallButton("Reset"))
		{
			x_aux = y_aux = z_aux = 0;
			x_aux2 = y_aux2 = z_aux2 = 0;
		}

		ImGui::End();
	}
}

