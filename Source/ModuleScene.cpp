#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Component.h"
#include "imgui_impl_sdl.h"
#include <stdlib.h>
#include <time.h>
#include <WinBase.h>
#include <string>
#include <sstream>

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

	octree.Create(float3::zero, float3::zero);
	octree.update_quadtree = true;
	draw_octree = false;

	App->camera->StartEditorCamera();
	//go_list.push_back(App->camera->editor_camera);
	//Load BakerHouse
	//App->loading_manager->Load(".//Assets//Models//warrior.FBX");
	App->loading_manager->Load(".//Assets//Models//Bakerhouse.fbx");
	App->loading_manager->unique_fbx_path = ".//Assets//Models//Street.fbx";

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
	if (octree.update_quadtree)
	{
		octree.min_point = float3::zero;
		octree.max_point = float3::zero;
		for (std::vector<Mesh*>::iterator it = octree_meshes.begin(); it != octree_meshes.end(); it++)
		{
			octree.Recalculate((*it)->bounding_box.minPoint, (*it)->bounding_box.maxPoint);
		}
		octree.Create(octree.min_point, octree.max_point);
		for (std::vector<Mesh*>::iterator it = octree_meshes.begin(); it != octree_meshes.end(); it++)
		{
			octree.Insert((*it));
		}
		octree.update_quadtree = false;
	}

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
			if (App->camera->editor_cam->IsGameObjectInFrustum(go_list[i]->GetBB(),go_list[i]->transform->trans_matrix_g.TranslatePart()))
				go_list[i]->Draw();
			else
				CONSOLE_LOG_INFO("DISCARDED %s", go_list[i]->GetName());
		}
		else if (!go_list[i]->IsRoot())
			go_list[i]->Draw();

	}
	//DrawOctree
	octree.DrawOctree(draw_octree);
	
	
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
	std::string new_name = "New GameObject ";

	new_name += std::to_string(id_new_go);

	tmp_GO->SetName(new_name.c_str());
	id_new_go++;
	tmp_GO->SetParent(scene_root);
	go_list.push_back(tmp_GO);
	tmp_GO->transform->UpdateTransformValues();


	return tmp_GO;
}

void ModuleScene::ClearScene()
{
	if (scene_root->HasChilds())
	{
		scene_root->ClearRelations();
		scene_root->childs_list.clear();
		go_list.clear();
	}
		
	App->loading_manager->unique_fbx_path = "";
	App->loading_manager->unique_material_path = "";
	imported_go = nullptr;
}

bool ModuleScene::HasObjects()
{
	return (go_list.size() > 1);
}

void ModuleScene::AddToOctree(GameObject * go)
{
	octree.Insert(go);
}

void ModuleScene::CollectOctreeIntersections(std::list<Mesh*>& item_elements, AABB* bounding_box)
{
	octree.CollectIntersections(item_elements, bounding_box);
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

void ModuleScene::DrawModuleConfig()
{
	if (ImGui::CollapsingHeader("Scene"))
	{
		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->input_tick_arr[0]);
		ImGui::Text("Octree");
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Checkbox("   DrawOctree", &draw_octree);

			ImGui::Text("   Static meshes:"); ImGui::SameLine();
			ImGui::Text(" %d", octree_meshes.size());

			ImGui::Text("   Min Point:"); ImGui::SameLine();
			ImGui::Text("   X:%d", octree.min_point.x); ImGui::SameLine();
			ImGui::Text("   Y:%d", octree.min_point.y); ImGui::SameLine();
			ImGui::Text("   Z:%d", octree.min_point.z);

			ImGui::Text("   Max Point:"); ImGui::SameLine();
			ImGui::Text("   X:%d", octree.max_point.x); ImGui::SameLine();
			ImGui::Text("   Y:%d", octree.max_point.y); ImGui::SameLine();
			ImGui::Text("   Z:%d", octree.max_point.z);
			ImGui::Spacing();
		ImGui::Text("Selected GameObject");
			ImGui::Separator();
			ImGui::Spacing();
			if (GetSelected() != nullptr)
			{
				ImGui::Text("  Name:"); ImGui::SameLine();
				ImGui::Text("%s", GetSelected()->GetName());
				if (GetSelected()->HasMesh())
				{
					ImGui::Text("   AABB MinPoint:");
					ImGui::Text(" X: %d", GetSelected()->GetBB().minPoint.x);
					ImGui::Text(" Y: %d", GetSelected()->GetBB().minPoint.y);
					ImGui::Text(" Z: %d", GetSelected()->GetBB().minPoint.z);
					ImGui::Text("   AABB Max Point:");
					ImGui::Text(" X: %d", GetSelected()->GetBB().maxPoint.x);
					ImGui::Text(" Y: %d", GetSelected()->GetBB().maxPoint.y);
					ImGui::Text(" Z: %d", GetSelected()->GetBB().maxPoint.z);
				}
		

			}
			else
				ImGui::Text("Select a GameObject to show its info");
		


		
	}
	else
		App->audio->input_tick_arr[0] = FALSEBOOL;

}

void ModuleScene::DrawChilds(GameObject* parent)
{
	if (parent == nullptr)
		return;
	// Flags ------------------

	uint flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (parent->IsSelected())
		flags |= ImGuiTreeNodeFlags_Selected;
	
	if (parent->GetNumChilds() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	// Draw line --------------
	bool opened = ImGui::TreeNodeEx(parent->GetName(),flags);
	
	// Set Selected on click
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

void ModuleScene::SaveScene(std::vector<GameObject*> go_list)
{
	//Example of a saved scene
	/*{"Game Objects":[
	{
		"UID":1642009359,
			"ParentUID" : 1619219037,
			"Name" : "RootNode",
			"Translation" : [0, 0, 0],
			"Scale" : [1, 1, 1],
			"Rotation" : [0, 0, 0, 1],
			"Components" : []},
	{
	"UID":1336602188,
	"ParentUID" : 1642009359,
	"Name" : "RPG-Character-Mesh",

	"Translation" : [-0.178409,0,-0.079894],
	"Scale" : [1,1,1],
	"Rotation" : [-0.707107,0,0,0.707107],
	"Components" : [
		{
			"Type":1,
				"Resource" : 0,
				"Alpha Test" : 0.500000,
				"Transform" : [1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]},
	{
	"Type":0,
	"Resource" : 1784,
	"Bones Root" : 0,
	"Tint" : [1,1,1,1] }
	] }
	*/
	FILE* fp = fopen("Assets/Settings/scene1.json", "wb"); // non-Windows use "w"
	char writeBuffer[10000];
	Document savescene_w;
	savescene_w.SetObject();
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	Document::AllocatorType& allocator = savescene_w.GetAllocator();

	Value scene(kObjectType);
	for (std::vector<GameObject*>::iterator it = go_list.begin(); it != go_list.end(); it++)
	{
		if ((*it)->childs_list.size() > 0)
		{					
			scene.AddMember("GameObject", SaveGO((*it), allocator), allocator);
		}
	}
	savescene_w.AddMember("Scene1", scene, allocator);

	Writer<FileWriteStream> writer(os);
	savescene_w.Accept(writer);
	fclose(fp);
	App->imgui->want_to_save = false;
}
Value ModuleScene::SaveGO(GameObject* go, Document::AllocatorType& allocator)
{
	Value data_go(kObjectType);

	//Name of the object
	Value name(go->name.c_str(), allocator);
	data_go.AddMember("name", name, allocator);

	//Components of the object
	if (go->components_list.size() > 0)
	{
		Value comp(kObjectType);
		Value comp_transform(kArrayType);
		for (int i = 0; i < go->components_list.size(); i++)
		{

			//Save name Component
			Value comp_name(go->components_list[i]->GetName(), allocator);
			comp.AddMember("name", comp_name, allocator);

			//Save type Component
			ComponentType check_type = go->components_list[i]->GetType();
			int hola = go->components_list[i]->GetType();
			std::string type;
			type.append(std::to_string(hola));
			Value type_trans(type.c_str(), allocator);
			comp.AddMember("type", type_trans, allocator);

			//Is active			
			bool active = go->components_list[i]->active;
			std::string s_active;
			s_active.append(std::to_string(active));
			Value comp_active(s_active.c_str(), allocator);
			comp.AddMember("active", comp_active, allocator);

			//Save owner Component
			GameObject* owner = go->components_list[i]->GetOwner();
			Value comp_owner(owner->GetName(), allocator);
			comp.AddMember("owner", comp_owner, allocator);

			switch (check_type)
			{
			case TRANSFORM:
			{

				Transform * trans = go->transform->GetTransform();
				comp_transform.PushBack((float)trans->pos.x, allocator);
				comp_transform.PushBack((float)trans->pos.y, allocator);
				comp_transform.PushBack((float)trans->pos.z, allocator);

				comp.AddMember("TRANSFORM", comp_transform, allocator);
			}
			break;
			default:
				break;
			}
			//Add components values to member components
			data_go.AddMember("Component", comp, allocator);
		}
	}
	



	

	return data_go;
}


