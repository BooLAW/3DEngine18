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
	JSON_Value* root_value;
	root_value = json_parse_file(path);
	if (root_value == NULL)
	{
		CONSOLE_LOG("Error Json file with path %s not found",path);
		return nullptr;
	}
	else
	{
		return root_value;
	}
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
