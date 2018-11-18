#include "Globals.h"
#include "Application.h"
#include "ModuleSceneImGui.h"
#include "ImGui/imgui.h"
#include "imgui_impl_sdl.h"
#include "ImGui/imgui_dock.h"
#include "ImGui/ImGuizmo.h"
#include "PCG/pcg_basic.h"

#include "PanelConsole.h"
#include "PanelConfiguration.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "ComponentMesh.h"
#include "PanelScene.h"
#include "PanelGame.h"
#include "Camera.h"
#include "ModuleCamera3D.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "Camera.h"

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
	ImGui::InitDock();
	//Add Panels HERE
	console = new PanelConsole();
	configuration = new PanelConfiguration();
	hierarchy = new PanelHierarchy();
	inspector = new PanelInspector();
	scene = new PanelScene();
	game = new PanelGame();

	panels.push_back(console);
	panels.push_back(configuration);
	panels.push_back(inspector);
	panels.push_back(hierarchy);
	panels.push_back(scene);
	panels.push_back(game);
	quit = false;
	App->profiler.SaveInitData("UI");

	guizmo_operation = TRANSLATE;
	guizmo_mode = LOCAL;
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
	if (quit)
		return UPDATE_STOP;

	ImGui_ImplSdl_NewFrame(App->window->window);
	ImGuizmo::BeginFrame();

	return UPDATE_CONTINUE;
}
// Update
update_status ModuleSceneGui::Update(float dt)
{
	App->profiler.StartTimer("UI");
	//RICARD: For a cleaner code, we decided to have a switch with the CreateMainMenu.
	CreateMainMenu();
	ChangeModeWithInput();
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
	flags |= ImGuiWindowFlags_NoScrollbar;


	float h_offset = 50.0f;
	float w_offset = 0.0f;


	DrawTools(flags);
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

void ModuleSceneGui::DrawImGuizmo()
{
	if (App->scene_intro->GetSelected() != nullptr )
	{
		GameObject* go_selected = App->scene_intro->GetSelected();
		ImVec2 pos = App->imgui->scene->GetPos();
		ImVec2 size = App->imgui->scene->GetSize();
		ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

		ComponentTransform* trans = (ComponentTransform*)go_selected->GetComponent(TRANSFORM);

		float4x4 vmat = App->camera->GetEditorCam()->GetViewMatrix4x4();

		float4x4 global = trans->trans_matrix_g;

		global.Transpose();

		ImGuizmo::Manipulate(&vmat[0][0], App->camera->GetEditorCam()->GetProjMatrix(), (ImGuizmo::OPERATION)guizmo_operation, (ImGuizmo::MODE::LOCAL), (float*)&global);
		
		global.Transpose();
		
		if (!App->scene_intro->GetSelected()->IsStatic() && !global.Equals(trans->trans_matrix_g))
		{
			trans->UpdateTransformFromGuizmo(global);
			//update childs
			
			if (go_selected->HasMesh())
			{
				go_selected->GetCMesh()->UpdateBoundingBox(trans->trans_matrix_g);
			}
			if (go_selected->HasChilds())
			{
				trans->UpdateBBChilds(go_selected);
			}
			if (go_selected->HasCam())
				go_selected->GetCCamera()->Update();
			

		}
		
	}
}

void ModuleSceneGui::DrawTools(uint flags)
{
	flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::SetNextWindowPos(ImVec2(0, 6));
	ImGui::SetNextWindowSize(ImVec2(App->window->width, 45));
	
	bool active = true;
	ImGui::Begin("Tool Buttons", &active, flags);
	//Guizmo
	{
		//TRANSLATE-------------------------
		bool change_color = false;

		if (guizmo_operation == TRANSLATE)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 0.3));
			change_color = true;
		}

		ImGui::SetCursorPos(ImVec2(10, 18));

		if (ImGui::Button("TRANS"))
			guizmo_operation = TRANSLATE;

		if (change_color == true)
		{
			ImGui::PopStyleColor(1);
			change_color = false;
		}
		ImGui::SameLine();


		//ROTATE--------------------------------
		if (guizmo_operation == ROTATE)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 0.3));
			change_color = true;
		}


		ImGui::SetCursorPos(ImVec2(60, 18));

		if (ImGui::Button("ROTATE"))
			guizmo_operation = ROTATE;

		if (change_color == true)
		{
			ImGui::PopStyleColor(1);
			change_color = false;

		}
		ImGui::SameLine();
		//SCALE-------------------------------
		if (guizmo_operation == SCALE)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 0.3));
			change_color = true;

		}


		ImGui::SetCursorPos(ImVec2(115, 18));
		if (ImGui::Button("SCALE"))
			guizmo_operation = SCALE;

		if (change_color == true)
		{
			ImGui::PopStyleColor(1);
			change_color = false;

		}
		ImGui::SameLine();
		bool change_mode_color = false;
		//LOCAL-----------------------------------
		if (guizmo_mode == LOCAL)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 0.3));
			change_mode_color = true;
		}


		ImGui::SetCursorPos(ImVec2(250, 18));
		if (ImGui::Button("LOCAL"))
			guizmo_mode = LOCAL;
		if (change_mode_color == true)
		{
			ImGui::PopStyleColor(1);
			change_mode_color = false;
		}
		ImGui::SameLine();
		//WORLD----------------------------------
		if (guizmo_mode == WORLD)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 0.3));
			change_mode_color = true;
		}


		ImGui::SetCursorPos(ImVec2(300, 18));
		if (ImGui::Button("WORLD"))
			guizmo_mode = WORLD;
		if (change_mode_color == true)
		{
			ImGui::PopStyleColor(1);
			change_mode_color = false;

		}
	}
	//Play & Pause
	{
		bool change_state = false;
		//PLAY
		if (App->state == playing)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 0.3));
			change_state = true;
		}

		ImGui::SetCursorPos(ImVec2(600, 18));
		if (ImGui::Button("PLAY"))
		{
			App->time_manager->Play();
			App->state = playing;
		}
	
		if (change_state == true)
		{
			ImGui::PopStyleColor(1);
			change_state = false;
		}
		//PAUSE
		if (App->state == paused)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 0.3));
			change_state = true;
		}
		ImGui::SetCursorPos(ImVec2(650, 18));

		if (ImGui::Button("PAUSE"))
		{
			App->time_manager->Pause();
			App->state = paused;
			

		}
		if (change_state == true)
		{
			ImGui::PopStyleColor(1);
			change_state = false;
		}
		//STOP
		if (App->state == stopped)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 0.3));
			change_state = true;
		}
		ImGui::SetCursorPos(ImVec2(720, 18));

		if (ImGui::Button("STOP"))
		{
			App->time_manager->Stop();
			App->state = stopped;
		
		}
		if (change_state == true)
		{
			ImGui::PopStyleColor(1);
			change_state = false;
		}
		ImGui::SameLine();
		int non_decimal_part = App->time_manager->game_time / 1000;
		int decimal_part = App->time_manager->game_time - (1000 * non_decimal_part);
		ImGui::Text("Game Time: %d.%d", non_decimal_part, decimal_part);
		ImGui::SameLine();
		//ImGui::Set
		ImGui::Text("  Time step");
		ImGui::SameLine();
		ImGui::PushItemWidth(50);
		ImGui::DragFloat("s", &App->time_manager->time_scale,0.1,0.1,1.0);
		ImGui::SameLine();
		ImGui::VerticalSeparator();
		ImGui::SameLine();
		ImGui::Checkbox("DrawOctree", &App->scene_intro->draw_octree);

	}
	ImGui::End();
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
	style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.68f, 0.44f, 0.44f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.41f, 0.05f, 0.01f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.41f, 0.05f, 0.01f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.41f, 0.05f, 0.01f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.62f, 0.21f, 0.21f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.72f, 0.65f, 0.21f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.70f, 0.03f, 0.03f, 0.40f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.67f, 0.40f, 0.40f, 0.60f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.40f, 0.40f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.72f, 0.08f, 0.08f, 0.45f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.14f, 0.14f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.21f, 0.29f, 0.80f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.86f, 0.02f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.86f, 0.02f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.96f, 0.93f, 0.07f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

}

void ModuleSceneGui::ChangeModeWithInput()
{
	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
		guizmo_operation = TRANSLATE;
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
		guizmo_operation = ROTATE;
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		guizmo_operation = SCALE;
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


void ModuleSceneGui::CreateMainMenu()
{
	// Menu
	static bool show_app_main_menu_bar = true;
	if (show_app_main_menu_bar)
	{
		if (ImGui::BeginMainMenuBar())
		{
			
			if (ImGui::BeginMenu("File"))
			{
				App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[0]);
				if (ImGui::MenuItem("Quit", "Alt + F4"))
				{		
					quit = true;
				}
					
				if (ImGui::MenuItem("Save")) 
				{ 
					App->scene_intro->SaveScene(App->scene_intro->go_list);
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[1]);
					want_to_save = true; 
				}
				else
					App->audio->menu_tick_arr[1] = FALSEBOOL;
				
				if (ImGui::MenuItem("Load")) 
				{ 
					App->scene_intro->LoadScene(".\\Assets\\Scenes\\scene1.json");
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[2]);
					want_to_load = true; 
				}
				else
					App->audio->menu_tick_arr[2] = FALSEBOOL;
				ImGui::EndMenu();
			}
			else
				App->audio->menu_tick_arr[0] = FALSEBOOL;
			if (ImGui::BeginMenu("Tools")) 
			{
				App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[3]);
				if (ImGui::MenuItem("ImGui Demo"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[4]);
					ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_Always);
					show_test_window = !show_test_window;
				}
				else
					App->audio->menu_tick_arr[4] = FALSEBOOL;
				if (ImGui::MenuItem("Random Number Generator"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[5]);
					show_random_number_generator = !show_random_number_generator;
				}
				else
					App->audio->menu_tick_arr[5] = FALSEBOOL;			
				if (ImGui::MenuItem("Profiler"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[8]);
					show_profiler = !show_profiler;
				}
				else
					App->audio->menu_tick_arr[8] = FALSEBOOL;
				if (ImGui::MenuItem("Check Collisions"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[9]);
					App->physics->GetSphereCollisions();
					App->physics->GetCubeCollisions();
				}
				else
					App->audio->menu_tick_arr[9] = FALSEBOOL;
				ImGui::EndMenu(); 
			}
			else
				App->audio->menu_tick_arr[3] = FALSEBOOL;
			if (ImGui::BeginMenu("View"))
			{
				App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[10]);
				if (ImGui::MenuItem("Only Scene"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[11]);
					ShowOnlyScene();
				}
				else
					App->audio->menu_tick_arr[11] = FALSEBOOL;
				if (ImGui::MenuItem("Standard Panels"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[12]);
					ShowSTDPanels();
				}
				else
					App->audio->menu_tick_arr[12] = FALSEBOOL;
				if (ImGui::MenuItem("Console"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[13]);
					console->Activate();
				}
				else
					App->audio->menu_tick_arr[13] = FALSEBOOL;
				if (ImGui::MenuItem("Configuration"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[14]);
					configuration->Activate();
				}
				else
					App->audio->menu_tick_arr[14] = FALSEBOOL;
		
				if (ImGui::MenuItem("Hierarchy"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[15]);
					hierarchy->Activate();
				}
				else
					App->audio->menu_tick_arr[15] = FALSEBOOL;
				if (ImGui::MenuItem("Inspector"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[16]);
					inspector->Activate();
				}
				else
					App->audio->menu_tick_arr[16] = FALSEBOOL;
				ImGui::EndMenu();
			}
			else
				App->audio->menu_tick_arr[10] = FALSEBOOL;
			if (ImGui::BeginMenu("About..."))
			{
				App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[17]);
				//put links in  our names
				ImGui::Text("Living Worlds");
				ImGui::Text("by Pau Bonet");
				ImGui::SameLine();
				if (ImGui::Button("REPO"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[18]);
					App->OpenWebPage("https://github.com/BooLAW");
				}
				else
					App->audio->menu_tick_arr[18] = FALSEBOOL;				
				ImGui::SameLine();
				ImGui::Text("& Josep Pi");
				ImGui::SameLine();
				if (ImGui::Button("REPO "))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[19]);
					App->OpenWebPage("https://github.com/joseppi");
				}
				else
					App->audio->menu_tick_arr[19] = FALSEBOOL;
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
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[20]);
					App->OpenWebPage("https://github.com/BooLAW/3DEngine18");
				}
				else
					App->audio->menu_tick_arr[20] = FALSEBOOL;
				if (ImGui::MenuItem("Link to Wiki"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[21]);
					App->OpenWebPage("https://github.com/BooLAW/3DEngine18/wiki");
				}
				else
					App->audio->menu_tick_arr[21] = FALSEBOOL;
				if (ImGui::MenuItem("Report a Bug"))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[22]);
					App->OpenWebPage("https://github.com/BooLAW/3DEngine18/issues");
				}
				else
					App->audio->menu_tick_arr[22] = FALSEBOOL;
				if (ImGui::BeginMenu("Used Libraries",true))
				{
					App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[23]);
					if (ImGui::BeginMenu("ImGui"))
					{
						App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[24]);
						if (ImGui::MenuItem("Source Code"))
						{
							App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[25]);
							App->OpenWebPage("https://github.com/ocornut/imgui");
						}
						else
							App->audio->menu_tick_arr[25] = FALSEBOOL;
						if (ImGui::MenuItem("Docking Code"))
						{
							App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[26]);
							App->OpenWebPage("https://github.com/ocornut/imgui/tree/docking");
						}
						else
							App->audio->menu_tick_arr[26] = FALSEBOOL;
						ImGui::EndMenu();
					}
					else
						App->audio->menu_tick_arr[24] = FALSEBOOL;
					
					if (ImGui::MenuItem("PCG"))
					{
						App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[27]);
						App->OpenWebPage("http://www.pcg-random.org/download.html");
					}
					else
						App->audio->menu_tick_arr[27] = FALSEBOOL;
					if (ImGui::MenuItem("MathGeolib"))
					{
						App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[28]);
						App->OpenWebPage("https://github.com/juj/MathGeoLib");
					}
					else
						App->audio->menu_tick_arr[28] = FALSEBOOL;
					if (ImGui::MenuItem("STL"))
					{
						App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[29]);
						App->OpenWebPage("http://www.cplusplus.com/reference/stl/");
					}
					else
						App->audio->menu_tick_arr[29] = FALSEBOOL;
					if (ImGui::MenuItem("DevIL"))
					{
						App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[30]);
						App->OpenWebPage("http://openil.sourceforge.net/download.php");
					}
					else
						App->audio->menu_tick_arr[30] = FALSEBOOL;
					if (ImGui::MenuItem("Assimp"))
					{
						App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[31]);
						App->OpenWebPage("https://github.com/assimp/assimp/releases/tag/v4.1.0/");
					}
					else
						App->audio->menu_tick_arr[31] = FALSEBOOL;
					if (ImGui::MenuItem("OpenGL"))
					{
						App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[32]);
						App->OpenWebPage("https://www.opengl.org/documentation/");
					}
					else
						App->audio->menu_tick_arr[32] = FALSEBOOL;
					if (ImGui::MenuItem("MMGR"))
					{
						App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->menu_tick_arr[33]);
						App->OpenWebPage("https://github.com/ConfettiFX/The-Forge/tree/master/Common_3/ThirdParty/OpenSource/FluidStudios/MemoryManager");
					}
					else
						App->audio->menu_tick_arr[33] = FALSEBOOL;
					ImGui::EndMenu();
				}
				else
					App->audio->menu_tick_arr[23] = FALSEBOOL;
				ImGui::EndMenu();
			}
			else
				App->audio->menu_tick_arr[17] = FALSEBOOL;
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
	ImGui::ShowTestWindow();
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
	}
	ImGui::End();
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
	}
	ImGui::End();
	
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
	}
	ImGui::End();
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