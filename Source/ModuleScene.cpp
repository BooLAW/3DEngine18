#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Component.h"
#include "imgui_impl_sdl.h"
#include <stdlib.h>
#include <time.h>
#include <WinBase.h>

#include "ModuleJSON.h"
#include "Transform.h"
#include "ComponentTransform.h"
#include "Primitive.h"
#include "Camera.h"
#include "TextureMSAA.h"
#include "ComponentCamera.h"
#include "GameObject.h"

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
	float3 root_pos = float3::zero;
	Quat root_rotation = Quat::identity;
	float3 root_scale = float3::one;
	scene_root->transform->SetTransform(root_pos, root_rotation, root_scale);
	scene_root->transform->SetGlobalPos({ 0, 0, 0 });
	scene_root->transform->UpdateTransformValues();

	

	go_list.push_back(scene_root);
	
	App->camera->StartEditorCamera();
	//go_list.push_back(App->camera->editor_camera);
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
		if (go_list[i]->HasMesh())
		{
			if (App->camera->editor_cam->IsGameObjectInFrustum(go_list[i]->GetBB()))
				go_list[i]->Draw();
			else
				CONSOLE_LOG_INFO("DISCARDED %d", i);
		}
			
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
	tmp_GO->transform->UpdateTransformValues();


	return tmp_GO;
}

void ModuleScene::ClearScene()
{
	go_list.clear();
	//go_list.push_back(App->camera->editor_camera);
	App->loading_manager->unique_fbx_path = "";
	App->loading_manager->unique_material_path = "";
	imported_go = nullptr;
}

bool ModuleScene::HasObjects()
{
	return (go_list.size() > 1);
}

void ModuleScene::DrawInspector()
{

}

void ModuleScene::DrawHierarchy()
{
	if (!go_list.empty())
	{
		std::vector<GameObject*> root_childs = scene_root->childs_list;
		for (std::vector<GameObject*>::iterator it = root_childs.begin(); it != root_childs.end(); it++)
		{
			DrawChilds((*it));
		}

	}
}

void ModuleScene::DrawChilds(GameObject* parent)
{
	if (parent == nullptr)
		return;

	// Flags ------------------

	uint flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (parent->IsActive())
		flags |= ImGuiTreeNodeFlags_Selected;

	if (parent->GetNumChilds() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	// ------------------------
	// Draw line --------------
	// ------------------------
	bool opened = ImGui::TreeNodeEx(parent->GetName(), flags);

	// ------------------------
	// Input ------------------
	// ------------------------
	if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
	{
			if (!ImGui::IsItemClicked(1) || !parent->IsSelected())
			{
				ResetSelected();
				parent->SetSelected(true);
			}
		
	}
	// Draw childs recursive --
	if (opened)
	{
		std::vector<GameObject*> childs = parent->childs_list;

		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); it++)
		{
			DrawChilds(*it);
		}

		ImGui::TreePop();
	}
}

void ModuleScene::ResetSelected()
{
	for (std::vector<GameObject*>::iterator it = go_list.begin(); it != go_list.end(); it++)
	{
		(*it)->SetSelected(false);
	}
}

GameObject * ModuleScene::GetSelected()
{
	GameObject* aux = nullptr;
	for (std::vector<GameObject*>::iterator it = go_list.begin(); it != go_list.end(); it++)
	{
		if ((*it)->IsSelected() == true)
			aux = (*it);
	}

	return aux;
}



