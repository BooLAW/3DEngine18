#include "Globals.h"
#include "Application.h"
#include "ModuleSceneImGui.h"
#include "ImGui/imgui.h"
#include "imgui_impl_sdl.h"
#include "ImGui/imgui_dock.h"
#include "PCG/pcg_basic.h"

#include "PanelConsole.h"
#include "PanelConfiguration.h"
#include "PanelHierarchy.h"
#include "PanelComponents.h"
#include "PanelInspector.h"
#include "ComponentMesh.h"
#include "PanelScene.h"

ModuleSceneGui::ModuleSceneGui(bool start_enabled) : Module( start_enabled)
{
	
}

ModuleSceneGui::~ModuleSceneGui()
{}

// Load assets
bool ModuleSceneGui::Init()
{
	CONSOLE_LOG_INFO("Loading gui assets");
	App->profiler.StartTimer("UI");
	bool ret = true;
	ImGui_ImplSdl_Init(App->window->window);
	SetImguiStyle();
	ImGui::InitDock();
	//Add Panels HERE
	console = new PanelConsole();
	configuration = new PanelConfiguration();
	components = new PanelComponents();
	//hierarchy = new PanelHierarchy();
	inspector = new PanelInspector();
	scene = new PanelScene();

	panels.push_back(console);
	panels.push_back(configuration);
	panels.push_back(inspector);
	panels.push_back(components);
	//panels.push_back(hierarchy);
	panels.push_back(scene);
	App->profiler.SaveInitData("UI");
	return ret;
}

// Load assets
bool ModuleSceneGui::CleanUp()
{
	CONSOLE_LOG_INFO("Unloading gui scene");
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
	
	
	return UPDATE_CONTINUE;
}
void ModuleSceneGui::DrawImGui() {
	App->renderer3D->SetUILights();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	flags |= ImGuiWindowFlags_NoResize;

	float h_offset = 6.0f;
	float w_offset = 0.0f;

	ImGui::SetNextWindowPos(ImVec2(w_offset, h_offset));
	ImGui::SetNextWindowSize(ImVec2(App->window->width - w_offset, App->window->height - h_offset));


	if (ImGui::Begin("Dock", 0, flags))
	{
		ImGui::BeginDockspace();

		BlitPanels();

		ImGui::EndDockspace();
	}
	ImGui::End();

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



		Value n((*item)->panel_name, config_w.GetAllocator());
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
		Value n((*item)->panel_name, ret.GetAllocator());
		(*item)->render_pos.x = ret[n]["posx"].GetFloat();
		(*item)->render_pos.y = ret[n]["posy"].GetFloat();
		(*item)->render_size.x = ret[n]["sizex"].GetFloat();
		(*item)->render_size.y = ret[n]["sizey"].GetFloat();
		isBlitedDisplay = 0;
	}

	return true;
}

void ModuleSceneGui::SetImguiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 0.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ModuleSceneGui::ShowOnlyScene()
{
	for (std::vector<Panel*>::iterator item = panels.begin(); item != panels.end(); ++item)
	{
		if ((*item)->panel_name != "Scene")
		{
			(*item)->DeActivate();
		}
	}
}

void ModuleSceneGui::ShowSTDPanels()
{
	for (std::vector<Panel*>::iterator item = panels.begin(); item != panels.end(); ++item)
	{
		if ((*item)->panel_name != "Scene")
		{
			(*item)->Activate();
		}
	}
}


void ModuleSceneGui::Log(const std::string text,int type)
{
	if (console != nullptr)
		console->LogToConsole(text,type);
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
				if (ImGui::MenuItem("Only Scene"))
				{
					ShowOnlyScene();
				}
				if (ImGui::MenuItem("Standard Panels"))
				{
					ShowSTDPanels();
				}
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
				/*if (ImGui::MenuItem("Hierarchy"))
				{
					hierarchy->Activate();
				}*/
				if (ImGui::MenuItem("Inspector"))
				{
					inspector->Activate();
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
				if (ImGui::Button("REPO "))
					App->OpenWebPage("https://github.com/joseppi");
				ImGui::Separator();				
				ImGui::Text("Living Worlds is a game engine to create 3D world with realisitic physics.");
				ImGui::Separator();
				ImGui::Text("MIT License");
				/*
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
				*/				
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
				if (ImGui::BeginMenu("Used Libraries",true))
				{
					if (ImGui::BeginMenu("ImGui"))
					{
						if (ImGui::MenuItem("Source Code"))
						{
							App->OpenWebPage("https://github.com/ocornut/imgui");
						}
						if (ImGui::MenuItem("Docking Code"))
						{
							App->OpenWebPage("https://github.com/ocornut/imgui/tree/docking");
						}
						ImGui::EndMenu();
					}
					
					if (ImGui::MenuItem("PCG"))
					{
						App->OpenWebPage("http://www.pcg-random.org/download.html");
					}
					if (ImGui::MenuItem("MathGeolib"))
					{
						App->OpenWebPage("https://github.com/juj/MathGeoLib");
					}
					if (ImGui::MenuItem("STL"))
					{
						App->OpenWebPage("http://www.cplusplus.com/reference/stl/");
					}
					if (ImGui::MenuItem("DevIL"))
					{
						App->OpenWebPage("http://openil.sourceforge.net/download.php");
					}
					if (ImGui::MenuItem("Assimp"))
					{
						App->OpenWebPage("https://github.com/assimp/assimp/releases/tag/v4.1.0/");
					}
					if (ImGui::MenuItem("OpenGL"))
					{
						App->OpenWebPage("https://www.opengl.org/documentation/");
					}
					if (ImGui::MenuItem("MMGR"))
					{
						App->OpenWebPage("https://github.com/ConfettiFX/The-Forge/tree/master/Common_3/ThirdParty/OpenSource/FluidStudios/MemoryManager");
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	if (show_panel_inspector)ShowInspector();
	if (show_test_window)showTestWindow();
	if (show_sphere_creator)showSphereCreator();
	if (show_cube_creator)showCubeCreator();
	if (show_profiler)App->profiler.Draw(show_profiler);
	if (show_random_number_generator)showRandomNumberGenerator();
}

void ModuleSceneGui::showTestWindow() 
{
	ImGui::ShowTestWindow();

}

void ModuleSceneGui::ShowInspector()
{
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
		ImGui::Text("Position");
		ImGui::InputInt("Position X", &x_aux);
		ImGui::InputInt("Position Y", &y_aux);
		ImGui::InputInt("Position Z", &z_aux);
		ImGui::Spacing();
		ImGui::InputInt("Size", &x_aux2);


		if (ImGui::SmallButton("Create"))
		{
			Mesh* new_mesh = new Mesh();
			new_mesh->DefineCubeVertices(float3(x_aux,y_aux,z_aux), x_aux2);
			ComponentMesh* new_cmp = new ComponentMesh();
			new_cmp->mesh = new_mesh;
			new_cmp->Enable();
			new_cmp->type = ComponentType::MESH;
			
			GameObject* tmp_go = App->scene_intro->CreateNewGameObject();
			tmp_go->components_list.push_back(new_cmp);
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
			(*item)->Draw();
		}
	}
	
}