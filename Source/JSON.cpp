#include "JSON.h"
#include "Globals.h"

JSON::JSON(bool enabled) : Module(enabled)
 {

}

JSON::~JSON()
{
}

bool JSON::Awake()
 {
	bool ret = true;
	
		LOG("JSON loading...");
	return ret;
	}

JSON_Value * JSON::LoadJSONFile(const char * path)
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

bool JSON::CleanUp()
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
