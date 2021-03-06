#include "Globals.h"
#include "ModuleScene.h"
#include "ModuleSceneImGui.h"
#include "PanelGame.h"
#include "imgui_impl_sdl.h"
#include <stdlib.h>
#include <time.h>
#include <WinBase.h>
#include <sstream>
#include <limits>

#include "ComponentConstraint.h"
#include "ComponentRigidBody.h"
#include "ComponentPlayerController.h"
#include "ComponentColliderSphere.h"
#include "ComponentColliderCube.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "Component.h"

#include "PhysBody.h"
#include "Transform.h"
#include "Material.h"
#include "Camera.h"
#include "Mesh.h"

#include "TextureMSAA.h"

#include "ImGui/ImGuizmo.h"

#define RADIUS 44

ModuleScene::ModuleScene( bool start_enabled) : Module( start_enabled)
{
}

ModuleScene::~ModuleScene()
{


}

// Load assets
bool ModuleScene::Start()
{
	CONSOLE_LOG_INFO("Loading Intro assets");
	App->profiler.SaveInitData("Scene");
	bool ret = true;

	//-----------------------------
	srand(time(NULL));

	//Creating Root Game object
	scene_root = new GameObject();
	scene_root->SetName("ROOT");
	scene_root->SetRootGoFlag(true);
	float3 root_pos = float3::zero;
	Quat root_rotation = Quat::identity;
	float3 root_scale = float3::one;
	scene_root->comp_transform->SetTransform(root_pos, root_rotation, root_scale);
	scene_root->comp_transform->CalculateGlobalMatrix();

	scene_root->SetUID(1355249013);
	scene_root->SetParentUID(NULL);
	scene_root->parent = NULL;
		
	go_list.push_back(scene_root);

	//Create octree
	octree.Create(float3::zero, float3::zero);
	octree.update_octree = true;
	draw_octree = false;

	//Creating Editor Camera
	App->camera->StartEditorCamera();
	//Init Controller Settings
	//Creating Game Camera
	NewMainCamera();

	//Create Library folder
	for (int i = 0; App->loading_manager->load_paths.size() > i; i++)
	{
		if (App->loading_manager->GetTermination(App->loading_manager->load_paths[i].c_str()) == "fbx" ||
			App->loading_manager->GetTermination(App->loading_manager->load_paths[i].c_str()) == "FBX")
		{
			App->loading_manager->CreateFolders(App->loading_manager->load_paths[i].c_str());
			App->loading_manager->mesh_loader->LoadMesh(App->loading_manager->load_paths[i].c_str(), false);
		}
	}

	//Load Str
	//App->loading_manager->Load(".\\Assets\\Models\\Street.fbx");
	//App->loading_manager->unique_fbx_path = ".\\Assets\\Models\\Street.fbx";

	//Load Str
	App->loading_manager->Load(".\\Assets\\Scenes\\scene1.json");
	App->loading_manager->unique_fbx_path = ".\\Assets\\Scenes\\scene1.json";
	
	App->profiler.SaveRunTimeData("Scene");
	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	CONSOLE_LOG_INFO("Unloading Intro scene");
	for (int i = 0; go_list.size() > i; i++)
	{

		delete go_list[i];
	}
	ClearScene();

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{	
	if (octree.update_octree)
	{
		octree.RefactorOctree();
		octree.update_octree = false;
	}
	if (App->state == playing)
		MoveCurrentCamera();
	DeleteGameObjectsInList();

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

	//Draw Static GameObjects
	for (std::list<GameObject*>::iterator it = octree_objects.end(); it != octree_objects.end(); it++)
	{
		if ((*it)->HasMesh())
		{
			//Check In Frustum
			if (App->camera->editor_cam->IsGameObjectInFrustum((*it)->GetBB(), (*it)->comp_transform->trans_matrix_g.TranslatePart()))
				(*it)->Draw();
			else
				CONSOLE_LOG_INFO("DISCARDED %s", (*it)->GetName());
		}
		else if (!(*it)->IsRoot())
			(*it)->Draw();
	}

	//Draw Dynamic GameObjects
	for (int i = 0; i < go_list.size(); i++)
	{
		if (go_list[i]->GetFirstUpdate() && go_list[i]->HasMesh())
		{
			go_list[i]->comp_transform->UpdateTransformValues();
			if (go_list[i]->HasMesh())
				go_list[i]->GetCMesh()->UpdateBoundingBox(go_list[i]->comp_transform->trans_matrix_g);
			go_list[i]->SetFirstUpdate(false);

		}
		if (!go_list[i]->IsStatic() &&go_list[i]->HasMesh())
		{
			//Check In Frustum
			if (App->camera->GetCurrentCam()->IsGameObjectInFrustum(go_list[i]->GetBB(),go_list[i]->comp_transform->trans_matrix_g.TranslatePart()))
				go_list[i]->Draw();
		}
		else if (!go_list[i]->IsRoot())
			go_list[i]->Draw();
	}
	
	//Octree
	octree.DrawOctree(draw_octree);
	if (App->camera->GetCurrentCam()->IsFrustumActive())
		App->camera->GetCurrentCam()->DrawFrustum();
	if(App->camera->draw_mouse_ray)
		App->camera->DrawRay();

	App->physics->UpdatePhysics();


}


void ModuleScene::OnCollision(PhysBody* body1, PhysBody* body2)
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
	tmp_GO->comp_transform->UpdateTransformValues();

	return tmp_GO;
}

void ModuleScene::ClearScene()
{

	if (scene_root->HasChilds())
	{
		scene_root->ClearRelations();
		scene_root->childs_list.clear();
		go_list.clear();		
		go_list.push_back(scene_root);
	}
	App->physics->primitive_list.clear();

	App->physics->CleanUp();
	App->physics->Init();
	App->physics->Start();
		
	App->loading_manager->unique_fbx_path = "";
	App->loading_manager->unique_material_path = "";
	imported_go = nullptr;
}
void ModuleScene::ClearSceneButton()
{
	if (scene_root->HasChilds())
	{
		scene_root->ClearRelations();
		scene_root->childs_list.clear();
		go_list.clear();
		go_list.push_back(scene_root);
		//main_camera_go = nullptr;
	}
	if (main_camera_go == nullptr)
	{
		//App->camera->current_game_camera = nullptr;
	}

	App->loading_manager->unique_fbx_path = "";
	App->loading_manager->unique_material_path = "";
	imported_go = nullptr;
}

void ModuleScene::DeleteGameObject(GameObject* go_to_delete)
{
	if (go_to_delete->HasPhysBody())
	{		
		go_to_delete->physbody->primitive_ptr->has_primitive_render = false;				
	}
	if (go_to_delete->HasColliderSphere())
	{
		App->physics->GetWorld()->removeRigidBody(go_to_delete->physbody->GetRigidBody());
	}
	if (go_to_delete->HasColliderCube())
	{
		App->physics->GetWorld()->removeRigidBody(go_to_delete->physbody->GetRigidBody());
	}
	if (go_to_delete->HasChilds())
	{
		for (std::vector<GameObject*>::iterator it = go_to_delete->childs_list.begin(); it != go_to_delete->childs_list.end(); it++)
		{
			DeleteGameObject((*it));
		}
	}
	to_delete.push_back(go_to_delete);
}

void ModuleScene::DeleteGameObjectsInList()
{
	for (std::vector<GameObject*>::iterator it = to_delete.begin(); it != to_delete.end(); it++)
	{
		(*it)->SetSelected(false);

		if ((*it)->parent != nullptr)
			(*it)->parent->DeleteChild((*it));

		//delete it from go_list
		for (std::vector<GameObject*>::iterator item = go_list.begin(); item != go_list.end(); item++)
		{
			if ((*item) == (*it))
			{
				go_list.erase(item);
				return;
			}
		}		
	}
	to_delete.clear();

}

bool ModuleScene::HasObjects()
{
	return (go_list.size() > 1);
}

GameObject* ModuleScene::CreateMainCamera()
{
	GameObject* main_camera_go = new GameObject();
	main_camera_go->SetName("Main Camera");
	ComponentCamera* cam_comp = new ComponentCamera();
	cam_comp->cam->SetFarPlane(1000);
	cam_comp->SetOwner(main_camera_go);
	cam_comp->cam->ActivateFrustum(true);
	main_camera_go->PushComponent(cam_comp);
	
	return main_camera_go;
}

GameObject * ModuleScene::CreateMainCamera(ComponentTransform * comp_trans)
{
	GameObject* main_camera_go = new GameObject();
	main_camera_go->SetName("Main Camera");
	main_camera_go->comp_transform = comp_trans;
	ComponentTransform* aux_transform = (ComponentTransform*)main_camera_go->components_list[0];
	aux_transform->SetTransform(main_camera_go->comp_transform->GetTransform()->pos, main_camera_go->comp_transform->GetTransform()->rot, main_camera_go->comp_transform->GetTransform()->scale);
	ComponentCamera* cam_comp = new ComponentCamera();
	cam_comp->cam->SetFarPlane(1000);
	cam_comp->SetOwner(main_camera_go);
	cam_comp->cam->ActivateFrustum(true);
	
	cam_comp->Update();
	main_camera_go->PushComponent(cam_comp);

	return main_camera_go;
}

void ModuleScene::AddToOctree(GameObject * go)
{
	octree.Insert(go);
	octree_objects.push_back(go);
}

void ModuleScene::RemoveFromOctree(GameObject * go)
{
	octree.Remove(go);
	octree_objects.remove(go);
}

void ModuleScene::CollectOctreeIntersections(std::list<Mesh*>& item_elements, AABB* bounding_box)
{
	octree.CollectIntersections(item_elements, bounding_box);
}

void ModuleScene::DrawHierarchy()
{
	if (!go_list.empty())
	{
		std::vector<GameObject*> root_childs;
		for (std::vector<GameObject*>::iterator it = go_list.begin(); it != go_list.end(); it++)
		{
			if ((*it)->IsRoot())
			{
				root_childs = (*it)->childs_list;
			}
		}		

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
		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->input_tick_arr[1]);
		ImGui::Text("Octree");
		ImGui::Separator();
		ImGui::Spacing();

			ImGui::Text("   Static meshes:"); ImGui::SameLine();
			ImGui::Text(" %d", octree_objects.size());

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
				ImGui::Text("%s", GetSelected()->GetName()); ImGui::SameLine();
		
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
		App->audio->input_tick_arr[1] = FALSEBOOL;

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
	if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1) )
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
		
	FILE* fp = fopen("Assets/Scenes/scene1.json", "wb"); // non-Windows use "w"
	scenewriteBuffer[10000];
	Document savescene_w;
	savescene_w.SetObject();
	FileWriteStream os(fp, scenewriteBuffer, sizeof(scenewriteBuffer));

	Document::AllocatorType& allocator = savescene_w.GetAllocator();

	Value scene(kObjectType);
	Value childs(kObjectType);
	for (std::vector<GameObject*>::iterator it = go_list.begin(); it != go_list.end(); it++)
	{
		if ((*it)->childs_list.size() > 0)
		{		
			for (int i = 0; i < (*it)->childs_list.size(); i++)
			{
				//Parent number
				static int parent_i = 0;
				static int child_i = 0;
				if ((*it)->GetChild(i)->childs_list.size() > 0)
				{
					++parent_i;
					std::string go_name;
					go_name.append("GameObject_");
					go_name.append(std::to_string(parent_i));
					Value v_go_name(go_name.c_str(), allocator);
					GameObject* go_childs = (*it)->GetChild(i);
					scene.AddMember(v_go_name, SaveGO(go_childs, allocator), allocator);
					child_i = 0;
				}
				else
				{
					//Child number based on parent					
					std::string go_name;
					go_name.append("GameObject_");
					go_name.append(std::to_string(parent_i));
					go_name.append("_");
					go_name.append(std::to_string(child_i));
					Value v_go_name(go_name.c_str(), allocator);
					GameObject* go_childs = (*it)->GetChild(i);
					scene.AddMember(v_go_name, SaveGO(go_childs, allocator), allocator);
					child_i++;
				}
			}
		}
	}
	savescene_w.AddMember("Scene1", scene, allocator);

	Writer<FileWriteStream> writer(os);
	savescene_w.Accept(writer);
	fclose(fp);
}
void ModuleScene::LoadScene(const char* path)
{
	App->scene_intro->go_list;
	App->scene_intro->ClearScene();
	
	FILE* fp = fopen(path, "rb"); // non-Windows use "w"
	Document docload_r;
	const int sizeofbuffer = sizeof(scenewriteBuffer);
	char scenereadBuffer[sizeofbuffer] = {};

	FileReadStream is(fp, scenereadBuffer, sizeof(scenereadBuffer));
	docload_r.ParseStream(is);
	Document::AllocatorType& allocator = docload_r.GetAllocator();

	for (Value::ConstMemberIterator go_itr = docload_r["Scene1"].MemberBegin(); go_itr != docload_r["Scene1"].MemberEnd(); ++go_itr)
	{
		//Create GameObject
		GameObject* new_go = new GameObject();
		
		for (Value::ConstMemberIterator m_go_itr = go_itr->value.MemberBegin(); m_go_itr != go_itr->value.MemberEnd(); ++m_go_itr)
		{
			//Iterate through values of GameObject
			if (strcmp(m_go_itr->name.GetString(), "name") == 0)
			{	
				std::string go_name;
				go_name.append(m_go_itr->value.GetString());
				if (strcmp(m_go_itr->value.GetString(), "ROOT") == 0) //if it's root
				{		
					
					new_go->SetRootGoFlag(true);
					new_go->SetName(m_go_itr->value.GetString());
				}
				else if (go_name.find("//") == 1)
				{
					std::string final_go_name;
					final_go_name = App->loading_manager->GetFileName(go_name.c_str());
					new_go->SetName(final_go_name.c_str());
				}									
				else
					new_go->SetName(m_go_itr->value.GetString());
			}
			else if (strcmp(m_go_itr->name.GetString(), "active_go") == 0)
			{
				new_go->SetActive(m_go_itr->value.GetBool());
			}
			else if (strcmp(m_go_itr->name.GetString(), "uid") == 0)
			{
				new_go->SetUID(m_go_itr->value.GetUint());
			}
			else if (strcmp(m_go_itr->name.GetString(), "parent_uid") == 0)
			{
				new_go->SetParentUID(m_go_itr->value.GetUint());
			}	
			const char* my_string2 = m_go_itr->name.GetString();
			if (m_go_itr->value.IsObject())
			{
				for (Value::ConstMemberIterator m_cmp_itr = m_go_itr->value.MemberBegin(); m_cmp_itr != m_go_itr->value.MemberEnd(); ++m_cmp_itr)
				{
	
					//Iterate through GameObjects Component values
					Component* aux_comp = new Component();
					if (strcmp(m_cmp_itr->name.GetString(), "active_comp") == 0)
					{
						if (m_cmp_itr->value.IsBool())
						{
							aux_comp->active = m_cmp_itr->value.GetBool();
						}						
					}
					else if (strcmp(m_cmp_itr->name.GetString(), "type") == 0)
					{
						ComponentType comp_type;
						int cmp_type = m_cmp_itr->value.GetInt();
						comp_type = (ComponentType)cmp_type;
					}
					else if (m_cmp_itr->value.IsArray())
					{		
						int i = 0;
						float stat[10] = {};
						for (Value::ConstValueIterator m_cmp_trans_itr = m_cmp_itr->value.Begin(); m_cmp_trans_itr != m_cmp_itr->value.End(); ++m_cmp_trans_itr)
						{
							stat[i] = m_cmp_trans_itr->GetFloat();
							i++;
						}
						float3 pos(stat[0], stat[1], stat[2]);
						Quat rot(stat[3], stat[4], stat[5], stat[6]);
						float3 scale(stat[7], stat[8], stat[9]);
						new_go->comp_transform->SetTransform(pos, rot, scale);
						
					}
					else if (strcmp(m_cmp_itr->name.GetString(), "MESH") == 0)
					{
						std::string file_path;						
						file_path.append(m_cmp_itr->value.GetString());
						
						//Set Folder Path
						std::string dir_name;
						dir_name = App->loading_manager->GetFolderNameLW(file_path.c_str());

						App->scene_intro->folder_path = dir_name;						
						App->loading_manager->mesh_loader->InitMesh(file_path, new_go);
						if(new_go->HasMesh())
							App->camera->AdaptCamera(new_go->GetBB(), new_go->comp_transform->transform->pos);
						else
							App->camera->AdaptCamera(new_go->comp_transform->transform->pos);

					}
					else if (strcmp(m_cmp_itr->name.GetString(), "MATERIAL") == 0)
					{
						aux_comp = new Component(MATERIAL);
						std::string file_path;
						
					}
					else if (strcmp(m_cmp_itr->name.GetString(), "CAMERA") == 0)
					{
						new_go = CreateMainCamera(new_go->comp_transform);

						new_go->SetParentUID(go_list[0]->GetUID());

						if (new_go->HasCam())
							App->camera->cams_list.push_back(new_go);
						App->camera->SetCurrentCam(new_go);
						App->camera->StartNewCamera();

					}
					else if (strcmp(m_cmp_itr->name.GetString(), "COLLIDERSPHERE") == 0)
					{
						float pspherematrix[16] = {};
						float offset[3] = { 0,0,0 };
						float radius = 1;
						float radius_scale = 1;
						PSphere* aux_sphere = nullptr;
						for (Value::ConstMemberIterator m_cmp_itr2 = m_cmp_itr->value.MemberBegin(); m_cmp_itr2 != m_cmp_itr->value.MemberEnd(); ++m_cmp_itr2)
						{
							
							if (m_cmp_itr2->value.IsArray())
							{
								int i = 0;
								for (Value::ConstValueIterator m_cmp_trans_itr = m_cmp_itr2->value.Begin(); m_cmp_trans_itr != m_cmp_itr2->value.End(); ++m_cmp_trans_itr)
								{
									pspherematrix[i] = m_cmp_trans_itr->GetFloat();
									i++;
								}
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "offsetx") == 0)
							{
								offset[0] = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "offsety") == 0)
							{
								offset[1] = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "offsetz") == 0)
							{
								offset[2] = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "radius") == 0)
							{
								radius = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "radius_scale") == 0)
							{
								radius_scale = m_cmp_itr2->value.GetFloat();
							}
							else
							{
								aux_sphere = new PSphere();
								aux_sphere->radius = radius;
								ComponentColliderSphere* aux_comp = new ComponentColliderSphere(new_go, aux_sphere);
								aux_comp->radius = radius_scale;
								new_go->physbody->GetRigidBody()->getCollisionShape()->setLocalScaling(btVector3(radius_scale, radius_scale, radius_scale));
								if (m_cmp_itr2->value.IsBool())
								{
									aux_comp->owner->physbody->primitive_ptr->has_primitive_render = m_cmp_itr2->value.GetBool();
									
								}
								else
								{
									aux_comp->owner->physbody->primitive_ptr->has_primitive_render = false;
								}
								new_go->PushComponent((Component*)aux_comp);								
								new_go->physbody->SetTransform(pspherematrix);
								new_go->physbody->primitive_ptr->scale = { radius_scale,radius_scale,radius_scale };
							}
						}
					}
					else if (strcmp(m_cmp_itr->name.GetString(), "COLLIDERCUBE") == 0)
					{

						float pcubematrix[16] = {};
						float offset[3] = { 0,0,0 };
						float3 dimension = { 0,0,0 };
						float3 dimension_scale = { 0,0,0 };
						PCube* aux_cube = nullptr;						
						for (Value::ConstMemberIterator m_cmp_itr2 = m_cmp_itr->value.MemberBegin(); m_cmp_itr2 != m_cmp_itr->value.MemberEnd(); ++m_cmp_itr2)
						{
							if (m_cmp_itr2->value.IsArray())
							{
								int i = 0;
								for (Value::ConstValueIterator m_cmp_trans_itr = m_cmp_itr2->value.Begin(); m_cmp_trans_itr != m_cmp_itr2->value.End(); ++m_cmp_trans_itr)
								{
									pcubematrix[i] = m_cmp_trans_itr->GetFloat();
									i++;
								}
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "offsetx") == 0)
							{
								offset[0] = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "offsety") == 0)
							{
								offset[1] = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "offsetz") == 0)
							{
								offset[2] = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "dimensionx") == 0)
							{
								dimension.x = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "dimensiony") == 0)
							{
								dimension.y = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "dimensionz") == 0)
							{
								dimension.z = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "scalex") == 0)
							{
								dimension_scale.x = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "scaley") == 0)
							{
								dimension_scale.y = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsNumber() && strcmp(m_cmp_itr2->name.GetString(), "scalez") == 0)
							{
								dimension_scale.z = m_cmp_itr2->value.GetFloat();
							}
							else
							{
								aux_cube = new PCube();
								aux_cube->dimensions = dimension;								
								ComponentColliderCube* aux_comp = new ComponentColliderCube(new_go, aux_cube);
								aux_comp->dimensions_component = dimension_scale;
								btVector3 aux_vec3(dimension_scale.x, dimension_scale.y, dimension_scale.z);
								
								new_go->physbody->GetRigidBody()->getCollisionShape()->setLocalScaling(aux_vec3);
								
								aux_comp->center_offset[0] = offset[0];
								aux_comp->center_offset[1] = offset[1];
								aux_comp->center_offset[2] = offset[2];		
				
								if (m_cmp_itr2->value.IsBool())
								{
									aux_comp->owner->physbody->primitive_ptr->has_primitive_render = m_cmp_itr2->value.GetBool();
								}
								else
								{
									aux_comp->owner->physbody->primitive_ptr->has_primitive_render = false;
								}
								

								new_go->PushComponent((Component*)aux_comp);
								new_go->physbody->SetTransform(pcubematrix);
								new_go->physbody->primitive_ptr->scale = dimension_scale;
							}
						}
					}
					else if (strcmp(m_cmp_itr->name.GetString(), "RIGIDBODY") == 0)
					{						
						for (Value::ConstMemberIterator m_cmp_itr2 = m_cmp_itr->value.MemberBegin(); m_cmp_itr2 != m_cmp_itr->value.MemberEnd(); ++m_cmp_itr2)
						{
							if (m_cmp_itr2->value.IsFloat())
							{
								new_go->physbody->primitive_ptr->mass = m_cmp_itr2->value.GetFloat();
							}
							else if (m_cmp_itr2->value.IsBool())
							{
								new_go->physbody->ActivateGravity(m_cmp_itr2->value.GetBool());
								ComponentRigidBody* aux_comp = new ComponentRigidBody(new_go);
								new_go->PushComponent((Component*)aux_comp);								
							}
						}
					}
					else if (strcmp(m_cmp_itr->name.GetString(), "CONTROLLER") == 0)
					{						 
						float* values = new float[5];
						for (Value::ConstMemberIterator m_cmp_itr2 = m_cmp_itr->value.MemberBegin(); m_cmp_itr2 != m_cmp_itr->value.MemberEnd(); ++m_cmp_itr2)
						{
							if (strcmp(m_cmp_itr2->name.GetString(), "speed") == 0)
							{
								values[0] = m_cmp_itr2->value.GetFloat();
							}
							else if (strcmp(m_cmp_itr2->name.GetString(), "shoot_force") == 0)
							{
								values[1] = m_cmp_itr2->value.GetFloat();
							}
							else if (strcmp(m_cmp_itr2->name.GetString(), "jump_force") == 0)
							{
								values[2] = m_cmp_itr2->value.GetFloat();
							}
							else if (strcmp(m_cmp_itr2->name.GetString(), "bullet_radius") == 0)
							{
								values[3] = m_cmp_itr2->value.GetFloat();
							}
							else if (strcmp(m_cmp_itr2->name.GetString(), "sensitivity") == 0)
							{
								values[4] = m_cmp_itr2->value.GetFloat();
								ComponentPlayerController* aux_cmp = new ComponentPlayerController(new_go, values);
								new_go->PushComponent((Component*)aux_cmp);

							}
						}
					}
				}
			}
		}
		if (!new_go->IsRoot())
		{
			go_list.push_back(new_go);
		}
		
	}

	//Add Parents and Childs
	for (std::vector<GameObject*>::iterator go_itr = go_list.begin(); go_itr != go_list.end(); go_itr++)
	{		
		(*go_itr)->SetParent(go_list, (*go_itr)->GetParentUID());
	}
	
	uint this_size = sizeof(App->scene_intro->go_list);
	//uint other_size = sizeof(App->scene_intro->go_list);
	fclose(fp);
}
Value ModuleScene::SaveGO(GameObject* go, Document::AllocatorType& allocator)
{
	Value data_go(kObjectType);

	//Name of the object
	Value name(go->name.c_str(), allocator);
	data_go.AddMember("name", name, allocator);
	data_go.AddMember("active_go", go->IsActive(), allocator);

	//UIDs
	data_go.AddMember("uid", go->GetUID(), allocator);
	data_go.AddMember("parent_uid", go->GetParentUID(), allocator);

	//Components of the object
	if (go->components_list.size() > 0)
	{	
		for (int i = 0; i < go->components_list.size(); i++)
		{
			Value arr_comp(kObjectType);
			//Is active			
			bool active_cmp = go->components_list[i]->active;
			arr_comp.AddMember("active_comp", active_cmp, allocator);

			//Create the name of the component
			std::string s_comp_name;
			s_comp_name.append("Component");
			s_comp_name.append("_");
			s_comp_name.append(std::to_string(i));
			Value v_comp_name(s_comp_name.c_str(), allocator);

			//Save type Component
			ComponentType check_type = go->components_list[i]->GetType();
			unsigned int hola = go->components_list[i]->GetType();
			arr_comp.AddMember("type", hola, allocator);

			//Get Specific Component Data based on type
			switch (check_type)
			{
			case TRANSFORM:
			{
				//Getting the data from the GO
				Value arr_comp_transform(kArrayType);
				Transform*  trans = go->comp_transform->GetTransform();
				arr_comp_transform.PushBack((float)trans->pos.x, allocator);
				arr_comp_transform.PushBack((float)trans->pos.y, allocator);
				arr_comp_transform.PushBack((float)trans->pos.z, allocator);

				arr_comp_transform.PushBack((float)trans->rot.x, allocator);
				arr_comp_transform.PushBack((float)trans->rot.y, allocator);
				arr_comp_transform.PushBack((float)trans->rot.z, allocator);
				arr_comp_transform.PushBack((float)trans->rot.w, allocator);

				arr_comp_transform.PushBack((float)trans->scale.x, allocator);
				arr_comp_transform.PushBack((float)trans->scale.y, allocator);
				arr_comp_transform.PushBack((float)trans->scale.z, allocator);

				//Adding data to the component
				arr_comp.AddMember("TRANSFORM", arr_comp_transform, allocator);
				data_go.AddMember(v_comp_name, arr_comp, allocator);
				break;
			}
			case MATERIAL:
			{
				Value obj_comp_material(kObjectType);
				ComponentMaterial* com_mat_aux = (ComponentMaterial*)go->components_list[i];
				Material* mat_aux = com_mat_aux->data;
				Value path_name(com_mat_aux->tex_path.c_str() , allocator);
				obj_comp_material.AddMember("path", path_name, allocator);
				obj_comp_material.AddMember("id", mat_aux->textures_id, allocator);
				obj_comp_material.AddMember("heigh", mat_aux->height, allocator);
				obj_comp_material.AddMember("width", mat_aux->width, allocator);


				//Adding data to the component
				arr_comp.AddMember("MATERIAL", obj_comp_material, allocator);
				data_go.AddMember(v_comp_name, arr_comp, allocator);				
				break;
			}
			case MESH:
			{
				Value obj_comp_mesh(kObjectType);
				ComponentMesh* com_mesh_aux = (ComponentMesh*)go->components_list[i];
				Mesh* mesh_aux = com_mesh_aux->mesh;
				Value mesh_name(mesh_aux->file_path.c_str(), allocator);
				
				//Adding data to the component
				arr_comp.AddMember("MESH", mesh_name, allocator);
				data_go.AddMember(v_comp_name, arr_comp, allocator);
				break;
			}
			case CAMERA:
			{
				Value obj_comp_camera(kObjectType);
				ComponentCamera* com_camera_aux = (ComponentCamera*)go->components_list[i];
				Camera* camera_aux = com_camera_aux->cam;
				//Value mesh_name(camera_aux->draw_frustum, allocator);
				obj_comp_camera.AddMember("darw_fustrum", camera_aux->IsFrustumActive(), allocator);
				obj_comp_camera.AddMember("aspect_ratio", camera_aux->GetAspectRatio(), allocator);
				obj_comp_camera.AddMember("far_plane", camera_aux->GetFarPlane(), allocator);
				obj_comp_camera.AddMember("near_plane", camera_aux->GetNearPlane(), allocator);
				obj_comp_camera.AddMember("vertical_fov", camera_aux->GetVerticalFOV(), allocator);
				obj_comp_camera.AddMember("game_camera", com_camera_aux->game_camera, allocator);

				//Adding data to the component
				arr_comp.AddMember("CAMERA", obj_comp_camera, allocator);
				data_go.AddMember(v_comp_name, arr_comp, allocator);

				break;
			}
			case COLLIDERSPHERE:
			{
				Value obj_comp_collider(kObjectType);
				ComponentColliderSphere* com_collider_aux = (ComponentColliderSphere*)go->components_list[i];				
				PhysBody* aux_phybody = go->physbody;


				Value obj_ptransform_collider(kArrayType);
				float* phy_matrix = new float[16];
				aux_phybody->GetTransform(phy_matrix);
				float arr_matrix[16];
				for (int i = 0; i < 16; i++)
				{
					obj_ptransform_collider.PushBack(phy_matrix[i],allocator);
				}
				
				obj_comp_collider.AddMember("PTRANSFORM", obj_ptransform_collider,  allocator);
				obj_comp_collider.AddMember("offsetx", com_collider_aux->center_offset[0], allocator);
				obj_comp_collider.AddMember("offsety", com_collider_aux->center_offset[1], allocator);
				obj_comp_collider.AddMember("offsetz", com_collider_aux->center_offset[2], allocator);
				PSphere* aux_sphere_data = (PSphere*)go->physbody->primitive_ptr;
				obj_comp_collider.AddMember("radius_scale", com_collider_aux->radius, allocator);				
				obj_comp_collider.AddMember("radius", aux_sphere_data->radius, allocator);
				obj_comp_collider.AddMember("render", aux_phybody->GetRender(), allocator);
				//shall never have rigid body


				//Adding data to the component
				arr_comp.AddMember("COLLIDERSPHERE", obj_comp_collider, allocator);
				data_go.AddMember(v_comp_name, arr_comp, allocator);
				break;
			}
			case COLLIDERCUBE:
			{
				Value obj_comp_collider(kObjectType);
				ComponentColliderSphere* com_collider_aux = (ComponentColliderSphere*)go->components_list[i];
				PhysBody* aux_phybody = go->physbody;


				Value obj_ptransform_collider(kArrayType);
				float* phy_matrix = new float[16];
				aux_phybody->GetTransform(phy_matrix);
				float arr_matrix[16];
				for (int i = 0; i < 16; i++)
				{
					obj_ptransform_collider.PushBack(phy_matrix[i], allocator);
				}

				obj_comp_collider.AddMember("PTRANSFORM", obj_ptransform_collider, allocator);		

				obj_comp_collider.AddMember("offsetx", com_collider_aux->center_offset[0], allocator);
				obj_comp_collider.AddMember("offsety", com_collider_aux->center_offset[1], allocator);
				obj_comp_collider.AddMember("offsetz", com_collider_aux->center_offset[2], allocator);

				PCube* aux_cube_data = (PCube*)go->physbody->primitive_ptr;
				
				obj_comp_collider.AddMember("dimensionx", aux_cube_data->dimensions.x, allocator);
				obj_comp_collider.AddMember("dimensiony", aux_cube_data->dimensions.y, allocator);
				obj_comp_collider.AddMember("dimensionz", aux_cube_data->dimensions.z, allocator);

				obj_comp_collider.AddMember("scalex", aux_cube_data->scale.x, allocator);
				obj_comp_collider.AddMember("scaley", aux_cube_data->scale.y, allocator);
				obj_comp_collider.AddMember("scalez", aux_cube_data->scale.z, allocator);

				obj_comp_collider.AddMember("render", aux_phybody->primitive_ptr->has_primitive_render, allocator);
				//shall never have rigid body


				//Adding data to the component
				arr_comp.AddMember("COLLIDERCUBE", obj_comp_collider, allocator);
				data_go.AddMember(v_comp_name, arr_comp, allocator);
				break;
			}
			case RIGIDBODY:
			{
				Value obj_comp_rigidbody(kObjectType);
				ComponentRigidBody* com_rigidbody_aux = (ComponentRigidBody*)go->components_list[i];
				PhysBody* aux_phybody = go->physbody;

				obj_comp_rigidbody.AddMember("mass", aux_phybody->primitive_ptr->mass, allocator);
				obj_comp_rigidbody.AddMember("use_gravity", aux_phybody->HasGravity(), allocator);

				//Adding data to the component
				arr_comp.AddMember("RIGIDBODY", obj_comp_rigidbody, allocator);
				data_go.AddMember(v_comp_name, arr_comp, allocator);
				break;
			}
			case CONTROLLER:
			{
				Value obj_comp_playercontroller(kObjectType);
				ComponentPlayerController* com_rigidbody_aux = (ComponentPlayerController*)go->components_list[i];
				
				obj_comp_playercontroller.AddMember("speed", com_rigidbody_aux->GetSpeed(), allocator);
				obj_comp_playercontroller.AddMember("shoot_force", com_rigidbody_aux->GetShootForce(), allocator);
				obj_comp_playercontroller.AddMember("jump_force", com_rigidbody_aux->GetJumpForce(), allocator);
				obj_comp_playercontroller.AddMember("bullet_radius", com_rigidbody_aux->GetBulletRadius(), allocator);
				obj_comp_playercontroller.AddMember("sensitivity", com_rigidbody_aux->GetPlayerSensitivity(), allocator);

				//Adding data to the component
				arr_comp.AddMember("CONTROLLER", obj_comp_playercontroller, allocator);
				data_go.AddMember(v_comp_name, arr_comp, allocator);
				break;
			}
			default:
				break;
			}
		}
	}
	return data_go;
}

GameObject * ModuleScene::GetClosestGO(std::vector<GameObject*> gameobjects, LineSegment line)
{
	float3 closest_point;
	float closest_distance = VERYFAR;
	GameObject* closest_go = nullptr;
	float distance;
	
	for (std::vector<GameObject*>::iterator it = gameobjects.begin(); it != gameobjects.end(); it++)
	{
		GameObject* go = (*it);

		ComponentMesh* mesh = go->GetCMesh();

		if (go->HasMesh())
		{
			float3 point = { 0,0,0 };

			if (mesh->FirstPoint(line, point, distance))
			{
				if (distance < closest_distance)
				{
					closest_distance = distance;
					closest_point = point;
					closest_go = go;
				}
			}
		}
	}
	return closest_go;
}

void ModuleScene::ClickSelection(LineSegment mouse_ray)
{
	std::vector<GameObject*> intersected_list;

	for (std::vector<GameObject*>::iterator it = go_list.begin(); it != go_list.end(); it++)
	{
		GameObject* go = (*it);

		if (!go->HasMesh())
		{
			continue;
		}

		bool collided = mouse_ray.Intersects(go->GetBB());

		if (collided)
		{
			intersected_list.push_back(go);
		}
	}
	

	GameObject* closestGo = GetClosestGO(intersected_list, mouse_ray);
	if (closestGo == nullptr)
		return;
	if(GetSelected() != nullptr)
		GetSelected()->SetSelected(false);
	closestGo->SetSelected(true);
}

void ModuleScene::NewMainCamera()
{	
	main_camera_go = CreateMainCamera();
	go_list.push_back(main_camera_go);
	scene_root->AddChild(main_camera_go);
	if (main_camera_go->HasCam())
		App->camera->cams_list.push_back(main_camera_go);

	App->camera->SetCurrentCam(main_camera_go);
	App->camera->StartNewCamera();
	//create Player Controller
	CrateBasePlayerController(main_camera_go);
	settings = new ControllerSettings();

	settings->bullet_radius = main_camera_go->GetController()->GetBulletRadius();
	settings->jump_force = main_camera_go->GetController()->GetJumpForce();
	settings->game_cam_speed = main_camera_go->GetController()->GetSpeed();
	settings->sensitivity = main_camera_go->GetController()->GetPlayerSensitivity();
	settings->force = main_camera_go->GetController()->GetShootForce();

}

void ModuleScene::MoveCurrentCamera()
{
	//WASD
	if (!App->camera->current_game_camera->IsStatic())
	{
		MoveCameraGO();
		//Mouse Rotation
		if (App->imgui->game->MouseOver())
			RotateCameraGO();
		if (main_camera_go->HasCam())
			main_camera_go->GetCCamera()->Update();
		App->physics->ShootSphere(settings->force,settings->bullet_radius);
		if (update_settings)
		{
			settings->bullet_radius = main_camera_go->GetController()->GetBulletRadius();
			settings->jump_force = main_camera_go->GetController()->GetJumpForce();
			settings->game_cam_speed = main_camera_go->GetController()->GetSpeed();
			settings->sensitivity = main_camera_go->GetController()->GetPlayerSensitivity();
			settings->force = main_camera_go->GetController()->GetShootForce();
			update_settings = false;
		}
	}
}

void ModuleScene::MoveCameraGO()
{
	Camera* curr_cam = main_camera_go->GetCamera();
	Transform* curr_trans = main_camera_go->comp_transform->GetTransform();
	
	float3 newPos(0, 0, 0);

	//Z
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		newPos += curr_cam->frustum.front * settings->game_cam_speed;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		newPos -= curr_cam->frustum.front * settings->game_cam_speed;

	
	//X														
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		newPos -= curr_cam->frustum.WorldRight() * settings->game_cam_speed;
	
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		newPos += curr_cam->frustum.WorldRight() * settings->game_cam_speed;

	
	//JUMP
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
		newPos.y += settings->jump_force;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
		newPos.y -= settings->jump_force;

	

		if (!newPos.IsZero() || main_camera_go->GetFirstUpdate())
		{
			curr_cam->frustum.Translate(newPos);
			curr_trans->SetPosition(curr_cam->frustum.pos.x, curr_cam->frustum.pos.y, curr_cam->frustum.pos.z);
			if (main_camera_go->HasPhysBody())
			{
				if (main_camera_go->HasColliderSphere())
				{
					main_camera_go->GetColliderSphere()->Update();
				}
				if (main_camera_go->HasColliderCube())
				{
					main_camera_go->GetColliderCube()->Update();
				}

			}
			main_camera_go->SetFirstUpdate(false);
		}

		
		main_camera_go->comp_transform->UpdateTransformValues();
		main_camera_go->comp_transform->updated_outside = false;
	

}

void ModuleScene::RotateCameraGO()
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
	{
		main_camera_go->GetCamera()->HandleMouse(settings->sensitivity*0.1);
		main_camera_go->GetCCamera()->user_rotate = true;
	}
	
}

void ModuleScene::CrateBasePlayerController(GameObject* parent)
{
	
	ComponentPlayerController* comp = new ComponentPlayerController(parent);
	
	parent->PushComponent(comp);
}

void ModuleScene::CreateDistanceJoint()
{
	GameObject* aux = CreateNewGameObject();
	ComponentConstraint* new_constraint = new ComponentConstraint(aux);
	aux->PushComponent(new_constraint);
	
}

std::list<const char*> ModuleScene::GetBodiesForJoints()
{
	std::list<const char*> ret;
	//iterate all active GO 
	for (std::vector<GameObject*>::iterator it = go_list.end(); it != go_list.end(); it++)
	{
		if ((*it)->IsActive())
		{
			//check if it has physbody
			if ((*it)->HasPhysBody())
				ret.push_back((*it)->name.c_str());
		}
		
	}
	//put in a list all the names 
	return ret;
}

ControllerSettings::ControllerSettings()
{
	 game_cam_speed = 0.5f;
	 jump_force = 0.3f;
	 sensitivity = 0.1f;
	 bullet_radius = 0.5f;
	 force = 30.0f;
}
