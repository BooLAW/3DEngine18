#pragma once
#include "Module.h"
#include "Globals.h"
#include <stdint.h>
#include "PCG/pcg_basic.h"

class PanelInspector;
class PanelConsole;
class PanelConfiguration;
class PanelApplication;
class PanelComponents;
class PanelScene;
//class PanelHierarchy;
class Panel;
class ModuleSceneGui : public Module
{
public:
	ModuleSceneGui(bool start_enabled = true);
	~ModuleSceneGui();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void CreateMainMenu();
	void showTestWindow();
	void ShowInspector();
	void showConfigurationWindow();
	void showConsoleWindow();
	void showSphereCreator();
	void showRandomNumberGenerator();
	void showCubeCreator();
	void ManageInput(SDL_Event* e)const;
	void Log(const std::string text,int type);
	void DrawImGui();
	void BlitPanels();
	bool Save(Document& config_w, FileWriteStream& os);
	bool Load(Document* config_r);
	void SetImguiStyle();

public:
	//flags
	bool show_test_window = false;
	bool show_panel_inspector = false;
	bool show_console = false;
	bool show_sphere_creator = false;
	bool show_cube_creator = false;
	bool show_random_number_generator = false;
	bool show_profiler = false;
	bool want_to_save = false;
	bool want_to_load = false;
	void ShowOnlyScene();
	void ShowSTDPanels();
	//Application
	bool isVsyncActive = false;
	bool quit = false;
	int fps_slider = 60;

	char* organization_name = "UPC CITM";
	char* engine_name = TITLE;
	int isBlitedDisplay = panels.size();
	ImVec2 panel_pos;
	ImVec2 panel_size;


	//Geometry Creator
	int rad_aux = 0;
	int x_aux = 0, y_aux = 0, z_aux = 0;
	int x_aux2 = 0, y_aux2 = 0, z_aux2 = 0;
	int aux1 = 0, aux2 = 100;
	int random_int = 0;
	pcg32_random_t rng = PCG32_INITIALIZER;
	float random_float = 0.0f;
	
	vec pos_aux;
	vec min_pos_aux;
	vec max_pos_aux;
public:
	std::vector<Panel*> panels;

public:
	PanelConsole * console = nullptr;
	PanelConfiguration* configuration = nullptr;
	PanelApplication* application = nullptr;
	PanelComponents* components = nullptr;
	//PanelHierarchy* hierarchy = nullptr;
	PanelInspector* inspector = nullptr;
	PanelScene* scene = nullptr;
	
};
