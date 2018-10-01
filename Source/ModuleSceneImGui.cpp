#include "Globals.h"
#include "Application.h"
#include "ModuleSceneImGui.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "PCG/pcg_basic.h"

#include "PanelConsole.h"
#include "PanelConfiguration.h"
#include "PanelHierarchy.h"
#include "PanelComponents.h"

ModuleSceneGui::ModuleSceneGui(bool start_enabled) : Module( start_enabled)
{
	
}

ModuleSceneGui::~ModuleSceneGui()
{}

// Load assets
bool ModuleSceneGui::Init()
{
	CONSOLE_LOG("Loading gui assets");
	App->profiler.StartTimer("UI");
	bool ret = true;
	ImGui_ImplSdl_Init(App->window->window);

	//Add Panels HERE
	console = new PanelConsole();
	configuration = new PanelConfiguration();
	components = new PanelComponents();
	hierarchy = new PanelHierarchy();

	panels.push_back(console);
	panels.push_back(configuration);
	//panels.push_back(components);
	//panels.push_back(hierarchy);
	App->profiler.SaveInitData("UI");
	return ret;
}

// Load assets
bool ModuleSceneGui::CleanUp()
{
	CONSOLE_LOG("Unloading gui scene");
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
	App->profiler.StartTimer("UI");
	//RICARD: For a cleaner code, we decided to have a switch with the CreateMainMenu.
	switch (CreateMainMenu())
	{
		case UPDATE_STOP:
		{
			return UPDATE_STOP;
		}
	}
	//ImGui::ShowMetricsWindow();
	//ImGui::ShowTestWindow();
	App->profiler.SaveRunTimeData("UI");
	return UPDATE_CONTINUE;
}

update_status ModuleSceneGui::PostUpdate(float dt)
{
	BlitPanels();
	
	return UPDATE_CONTINUE;
}
void ModuleSceneGui::DrawImGui() {
	App->renderer3D->SetUILights();
	ImGui::Render();
}

bool ModuleSceneGui::Save(Document & config_w, FileWriteStream & os)
{
	Document::AllocatorType& allocator = config_w.GetAllocator();
	for (std::vector<Panel*>::iterator item = panels.begin(); item != panels.end(); ++item)
	{

		Value test(kObjectType);

		test.AddMember("posx", (*item)->render_pos.x, allocator);
		test.AddMember("posy", (*item)->render_pos.y, allocator);
		test.AddMember("sizex", (*item)->render_size.x, allocator);
		test.AddMember("sizey", (*item)->render_size.y, allocator);



		Value n((*item)->name.c_str(), config_w.GetAllocator());
		config_w.AddMember(n, test,allocator);
	}

	return true;
}

bool ModuleSceneGui::Load(Document * config_r)
{
	Document ret;
	ret.Parse(App->loadBuf);
	ret.IsObject();
	for (std::vector<Panel*>::iterator item = panels.begin(); item != panels.end(); ++item)
	{
		Value n((*item)->name.c_str(), ret.GetAllocator());
		(*item)->render_pos.x = ret[n]["posx"].GetFloat();
		(*item)->render_pos.y = ret[n]["posy"].GetFloat();
		(*item)->render_size.x = ret[n]["sizex"].GetFloat();
		(*item)->render_size.y = ret[n]["sizey"].GetFloat();
		isBlitedDisplay = 0;
	}

	return true;
}


void ModuleSceneGui::Log(const std::string text)
{
	if (console != nullptr)
		console->LogToConsole(text);
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
				if (ImGui::MenuItem("Save")) { want_to_save = true; }
				if (ImGui::MenuItem("Load")) { want_to_load = true; }
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
				if (ImGui::MenuItem("Profiler"))
				{
					show_profiler = !show_profiler;
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
					console->Activate();
				}
				if (ImGui::MenuItem("Configuration"))
				{
					configuration->Activate();
				}
				if (ImGui::MenuItem("Components"))
				{
					components->Activate();
				}
				if (ImGui::MenuItem("Hierarchy"))
				{
					hierarchy->Activate();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("About..."))
			{
				//put links in  our names
				ImGui::Text("Living Worlds");
				ImGui::Text("by Pau Bonet");
				ImGui::SameLine();
				if (ImGui::Button("REPO"))
					App->OpenWebPage("https://github.com/BooLAW");
				ImGui::SameLine();
				ImGui::Text("& Josep Pi");
				ImGui::SameLine();
				if (ImGui::Button("REPO"))
					App->OpenWebPage("https://github.com/joseppi");
				ImGui::Text("License");
				ImGui::Separator();
				ImGui::Spacing();
				ImGui::Text("MIT License");
				ImGui::Spacing();
					ImGui::Text("Copyright(c) 2018 Pau Bonet Vall Llebrera & Josep Pi");
					ImGui::Spacing();
					ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
					ImGui::Text("of this software and associated documentation files(the Software), to deal");
					ImGui::Text("in the Software without restriction, including without limitation the rights");
					ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
					ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
					ImGui::Text("furnished to do so, subject to the following conditions :");
					ImGui::Spacing();
					ImGui::Text("The above copyright notice and this permission notice shall be included in all");
					ImGui::Text("copies or substantial portions of the Software.");
					ImGui::Spacing();
					ImGui::Text("THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
						ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
						ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
						ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
						ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
						ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
						ImGui::Text("SOFTWARE.");
						ImGui::Spacing();
						ImGui::Separator();
				ImGui::Text("Living Worlds is a game engine to create 3D world with realisitic physics.");
				ImGui::Spacing();
				ImGui::Separator();

				if (ImGui::MenuItem("Link to Repository"))
				{
					App->OpenWebPage("https://github.com/BooLAW/3DEngine18");
				}
				if (ImGui::MenuItem("Link to Wiki"))
				{
					App->OpenWebPage("https://github.com/BooLAW/3DEngine18/wiki");
				}
				if (ImGui::MenuItem("Report a Bug"))
				{
					App->OpenWebPage("https://github.com/BooLAW/3DEngine18/issues");
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	if (show_test_window)showTestWindow();
	if (show_sphere_creator)showSphereCreator();
	if (show_cube_creator)showCubeCreator();
	if (show_profiler)App->profiler.Draw(show_profiler);
	if (show_random_number_generator)showRandomNumberGenerator();
}

void ModuleSceneGui::showTestWindow() 
{
	ImGui::ShowTestWindow(&show_test_window);

}

void ModuleSceneGui::showConsoleWindow()
{

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
			random_int = (" %i", (int)pcg32_boundedrand_r(&rng, aux2) + aux1);			
		}
		ImGui::SameLine();
		ImGui::Text("%i", random_int);
		
		ImGui::Spacing();
		if (ImGui::SmallButton("Generate Random Float"))
		{
			random_float = ldexp(pcg32_random_r(&rng), -32);
		}
		ImGui::SameLine();
		ImGui::Text("%f", random_float);

		ImGui::End();
	}

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

void ModuleSceneGui::ManageInput(SDL_Event * e) const
{
	ImGui_ImplSdl_ProcessEvent(e);
}

void ModuleSceneGui::BlitPanels()
{
	for (std::vector<Panel*>::iterator item = panels.begin(); item != panels.end(); ++item)
	{
		if ((*item)->IsActive())
		{
			//if (isBlitedDisplay < panels.size())
			{
				ImGui::SetNextWindowPos((*item)->render_pos, ImGuiSetCond_Always);
				ImGui::SetNextWindowSize((*item)->render_size, ImGuiSetCond_Always);
				isBlitedDisplay++;
			}
			

			(*item)->Draw();
			
		}
	}
}