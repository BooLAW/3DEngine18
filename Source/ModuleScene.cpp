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
	CONSOLE_LOG("Loading Intro assets");
	App->profiler.SaveInitData("Scene");
	bool ret = true;
	{
		//App->json->CreateNewJSON("testconfig.json");
		/*json_file file1("testconfig.json");*/
		//int age_output = file1.ReadInt("age");

		//JSON_Value *user_data = json_parse_file("testconfig.json");
		//JSON_Object *root_object = json_value_get_object(user_data);
		//json_file* file2 = new json_file(user_data, root_object, "testconfig.json");
		//const char* name_output = file1.ReadString("name");
		//int age_output2 = file2->GetInt("age", 900);
		//int heigh_output = file2->GetInt("heigh", 900);
	}

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
	App->loading_manager->Load(".\\Assets\\Textures\\Baker_house.png");
	App->profiler.SaveRunTimeData("Scene");
	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	CONSOLE_LOG("Unloading Intro scene");
	return true;
}

void ModuleScene::rapidjsonexamplecode()
{
	FILE* fp = fopen("big.json", "rb"); // non-Windows use "r"
	char readBuffer[500];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	
	Document document;
	document.ParseStream(is);
	document.IsObject();

	//TEST START
	{
		//const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
		document.HasMember("hello");
		document["hello"].IsString();
		CONSOLE_LOG("hello = %s\n", document["hello"].GetString());
		//hello = world
		document["t"].IsBool();
		CONSOLE_LOG("t = %s\n", document["t"].GetBool() ? "true" : "false");
		//t=true;
		CONSOLE_LOG("n = %s\n", document["n"].IsNull() ? "null" : "?");
		document["i"].IsNumber();
		// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
		document["i"].IsInt();
		CONSOLE_LOG("i = %d\n", document["i"].GetInt());
		// Alternative (int)document["i"]
		document["pi"].IsNumber();
		document["pi"].IsDouble();
		CONSOLE_LOG("pi = %g\n", document["pi"].GetDouble());

		const Value& a = document["a"];
		a.IsArray();
		for (SizeType i = 0; i < a.Size(); i++) // Uses SizeType instead of size_t
			CONSOLE_LOG("a[%d] = %d\n", i, a[i].GetInt());
	}
	//TEST END


	fclose(fp);
	
	Document d;
	d.Parse(readBuffer);
	// ...
	FILE* fp2 = fopen("output.json", "wb"); // non-Windows use "w"
	char writeBuffer[500];

	FileWriteStream os(fp2, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer(os);

	Value& s = d["t"];
	s.SetBool(true);

	d.Accept(writer);
	fclose(fp2);
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


