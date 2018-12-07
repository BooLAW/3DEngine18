#include "Globals.h"
#include "ModuleScene.h"
#include "imgui_impl_sdl.h"
#include <stdlib.h>
#include <time.h>
#include <WinBase.h>
#include <sstream>
#include <limits>

#include "ComponentPhysBody.h"
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

	//Creating Game Camera
	main_camera_go = CreateMainCamera();
	go_list.push_back(main_camera_go);
	scene_root->AddChild(main_camera_go);
	if (main_camera_go->HasCam())
		App->camera->cams_list.push_back(main_camera_go);
	App->camera->SetCurrentCam(main_camera_go);
	App->camera->StartNewCamera();

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

	cube.dimensions = float3(5.0f, 5.0f, 5.0f);
	cube.SetPos(0, 10, 10);
	App->physics->AddBody(cube, 1);
	

	cube2.dimensions = float3(5.0f, 5.0f, 5.0f);
	cube2.SetPos(0, 30, 7.5f);
	App->physics->AddBody(cube2, 1);

	

	//Load Street
	/*App->loading_manager->Load(".\\Assets\\Models\\Street.fbx");
	App->loading_manager->unique_fbx_path = ".\\Assets\\Models\\Street.fbx";*/
	
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
		
	App->loading_manager->unique_fbx_path = "";
	App->loading_manager->unique_material_path = "";
	imported_go = nullptr;
}

void ModuleScene::DeleteGameObject(GameObject* go_to_delete)
{
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

void ModuleScene::MoveCurrentCamera()
{
	//WASD
	MoveCameraGO();
	//Mouse Rotation
	RotateCameraGO();
}

void ModuleScene::MoveCameraGO()
{
	Camera* curr_cam = main_camera_go->GetCamera();
	Transform* curr_trans = main_camera_go->comp_transform->GetTransform();
	bool cam_moved = false;
	//Z
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		float new_z = curr_trans->pos.z;
		new_z -= game_cam_speed;
		curr_trans->SetPosition(curr_trans->pos.x, curr_trans->pos.y, new_z);
		cam_moved = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		float new_z = curr_trans->pos.z;
		new_z += game_cam_speed;
		curr_trans->SetPosition(curr_trans->pos.x, curr_trans->pos.y, new_z);
		cam_moved = true;

	}
	//X														
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		float new_x = curr_trans->pos.x;
		new_x -= game_cam_speed;
		curr_trans->SetPosition(new_x, curr_trans->pos.y, curr_trans->pos.z);
		cam_moved = true;

	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		float new_x = curr_trans->pos.x;
		new_x += game_cam_speed;
		curr_trans->SetPosition(new_x, curr_trans->pos.y, curr_trans->pos.z);
		cam_moved = true;

	}
	//Y
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
	{
		float new_y = curr_trans->pos.y;
		new_y -= game_cam_speed;
		curr_trans->SetPosition(curr_trans->pos.x, new_y, curr_trans->pos.z);
		cam_moved = true;

	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
	{
		float new_y = curr_trans->pos.y;
		new_y += game_cam_speed;
		curr_trans->SetPosition(curr_trans->pos.x, new_y, curr_trans->pos.z);
		cam_moved = true;
	}
	
	if (cam_moved || main_camera_go->GetFirstUpdate())
	{
		main_camera_go->comp_transform->UpdateTransformValues();
	/*	if (main_camera_go->HasRigidBody())
		{
			main_camera_go->GetRigidBody()->UpdateTransform();
		}*/
		if (main_camera_go->HasCam())
			main_camera_go->GetCCamera()->Update();

		main_camera_go->SetFirstUpdate(false);
	}
	

}

void ModuleScene::RotateCameraGO()
{

}




