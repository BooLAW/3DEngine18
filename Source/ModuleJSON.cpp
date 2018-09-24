#include "ModuleJSON.h"
#include "Globals.h"

ModuleJSON::ModuleJSON(bool enabled) : Module(enabled)
 {

}

ModuleJSON::~ModuleJSON()
{
}

bool ModuleJSON::Awake()
 {
	bool ret = true;
	
		LOG("Module JSON loading...");
	return ret;
	}

JSON_Value * ModuleJSON::LoadJSONFile(const char * path)
{
	JSON_Value* root_value = json_parse_file(path);

	return root_value;

}

JSON_Value * ModuleJSON::CreateNewJSON(const char * path)
{
	JSON_Value* root_value = json_parse_file(path);
	const char* name = "josep";

	root_value = json_value_init_object();
	json_object_set_string(json_object(root_value), "name", "holaaa");
	json_serialize_to_file(root_value, path);
	name = json_object_get_string(json_object(root_value), "name");

	return root_value;
}

bool ModuleJSON::CleanUp()
{
	return false;
}

bool json_file::FindValue(const char * str, json_value_type type, int index) const
{
	return false;
}

JSON_Value * json_file::GetValue(const char * field, int index) const
{
	return nullptr;
}

json_file::json_file(JSON_Value * value, JSON_Object * object, const char * path)
{
	this->value = value;
	this->object = object;
	this->root = object;
	this->path = path;
}

json_file::json_file(JSON_Object * Entry):object(Entry)
{
}

json_file::json_file()
{
}

json_file::~json_file()
{
}
