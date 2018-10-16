#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"
#include "Component.h"
#include "imgui_impl_sdl.h"
#include <stdlib.h>
#include <time.h>
#include <WinBase.h>

#include "ModuleJSON.h"

#define RADIUS 44

ModuleScene::ModuleScene( bool start_enabled) : Module( start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	CONSOLE_LOG_INFO("Loading Intro assets");
	App->profiler.SaveInitData("Scene");
	bool ret = true;

	//-----------------------------
	srand(time(NULL));

	scene_root = new GameObject();
	scene_root->SetName("ROOT");
	scene_root->root_go = true;
	go_list.push_back(scene_root);

	App->camera->Move(vec3(1.0f, 5.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	//Load BakerHouse
	App->loading_manager->Load(".\\Assets\\Models\\BakerHouse.fbx");
	App->loading_manager->unique_fbx_path = ".\\Assets\\Models\\BakerHouse.fbx";
	App->loading_manager->Load(".\\Assets\\Textures\\Baker_house.png");
	App->loading_manager->unique_material_path = ".\\Assets\\Models\\BakerHouse.fbx";
	App->profiler.SaveRunTimeData("Scene");
	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	CONSOLE_LOG_INFO("Unloading Intro scene");
	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	
	return UPDATE_CONTINUE;
}
void ModuleScene::DrawGameObjects()
{
	
	//Base Plane
	if (App->renderer3D->show_plane == true)
	{
		PPlane base_plane(0, 1, 0, 0);
		base_plane.axis = true;
		base_plane.wire = false;
		base_plane.color = White;
		base_plane.Render();
	}
	for (int i = 0; i < go_list.size(); i++)
	{
		go_list[i]->Draw();
	}

}


void ModuleScene::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

int ModuleScene::RandInt(int min, int max)
{
	LCG aux;
	int ret;
	ret = aux.Int(min, max);
	return ret;
}

float ModuleScene::RandFloat(float min, float max)
{

	LCG aux;
	float ret;
	ret = aux.Float(min, max);
	return ret;
}

GameObject * ModuleScene::CreateNewGameObject()
{
	GameObject* tmp_GO = new GameObject();
	tmp_GO->SetParent(scene_root);
	go_list.push_back(tmp_GO);


	return tmp_GO;
}

void ModuleScene::ClearScene()
{
	go_list.clear();
	App->loading_manager->unique_fbx_path = "";
	App->loading_manager->unique_material_path = "";
	imported_go = nullptr;
}

void ModuleScene::DrawInspector()
{

}

void ModuleScene::DrawHierarchy()
{
	if (!go_list.empty())
	{

		for (int i = 0; i < go_list.size(); i++)
		{
			if (ImGui::TreeNode(go_list[i]->GetName()))
			{

				for (int j = 0; j < go_list[i]->components_list.size(); j++)
				{
					GameObject* aux = go_list[i];
					if (ImGui::TreeNode(aux->components_list[j]->GetName()))
					{



						ImGui::TreePop();

					}

				}
				ImGui::TreePop();

			}
		}
	}
}



