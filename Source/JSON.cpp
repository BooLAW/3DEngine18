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

json_file * JSON::LoadJSONFile(const char * path)
{
	JSON_Value* root_value;
	JSON_Array* commits;
	JSON_Array* commit;
	CONSOLE_LOG("hola");

	return nullptr;
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
