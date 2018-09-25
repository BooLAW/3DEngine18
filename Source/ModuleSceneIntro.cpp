#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#include "ImGui/imgui_impl_sdl.h"
#include <stdlib.h>
#include <time.h>

#include "ModuleJSON.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include <cstdio>

using namespace rapidjson;


#define RADIUS 44

ModuleSceneIntro::ModuleSceneIntro( bool start_enabled) : Module( start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	CONSOLE_LOG("Loading Intro assets");
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

	rapidjsonexamplecode();



	//-----------------------------
	srand(time(NULL));
	
	App->camera->Move(float3(1.0f, 5.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	CONSOLE_LOG("Unloading Intro scene");
	return true;
}

void ModuleSceneIntro::rapidjsonexamplecode()
{
	FILE* fp = fopen("big.json", "rb"); // non-Windows use "r"
	char readBuffer[500];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
	Document document;
	document.ParseStream(is);
	assert(document.IsObject());
	assert(document.HasMember("hello"));
	assert(document["hello"].IsString());
	CONSOLE_LOG("hello = %s\n", document["hello"].GetString());
	//hello = world
	assert(document["t"].IsBool());
	CONSOLE_LOG("t = %s\n", document["t"].GetBool() ? "true" : "false");
	//t=true;
	CONSOLE_LOG("n = %s\n", document["n"].IsNull() ? "null" : "?");


	assert(document["i"].IsNumber());
	// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
	assert(document["i"].IsInt());
	CONSOLE_LOG("i = %d\n", document["i"].GetInt());
	// Alternative (int)document["i"]
	assert(document["pi"].IsNumber());
	assert(document["pi"].IsDouble());
	CONSOLE_LOG("pi = %g\n", document["pi"].GetDouble());

	const Value& a = document["a"];
	assert(a.IsArray());
	for (SizeType i = 0; i < a.Size(); i++) // Uses SizeType instead of size_t
		CONSOLE_LOG("a[%d] = %d\n", i, a[i].GetInt());
	// ...
	fclose(fp);

	Document d;
	d.Parse(json);
	// ...
	FILE* fp2 = fopen("output.json", "wb"); // non-Windows use "w"
	char writeBuffer[500];

	FileWriteStream os(fp2, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer(os);
	Value& s = d["t"];
	s.SetBool(false);

	d.Accept(writer);
	fclose(fp2);
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Render the Plane
	/* PAU
	Plane p({0, 1, 0}, 0);
	p..axis = true;
	p.Render(); */

	return UPDATE_CONTINUE;
}



void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

int ModuleSceneIntro::RandInt(int min, int max)
{
	LCG aux;
	int ret;
	ret = aux.Int(min, max);
	return ret;
}

float ModuleSceneIntro::RandFloat(float min, float max)
{

	LCG aux;
	float ret;
	ret = aux.Float(min, max);
	return ret;
}



