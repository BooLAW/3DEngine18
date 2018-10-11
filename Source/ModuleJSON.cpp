#include "ModuleJSON.h"
#include "Globals.h"
#include <string>

ModuleJSON::ModuleJSON(bool enabled) : Module(enabled)
 {

}
ModuleJSON::~ModuleJSON()
{
}

bool ModuleJSON::Awake()
 {
	bool ret = true;
	
	CONSOLE_LOG("Module JSON loading...",INFO_LOG);
	return ret;
	}


JSON_Value * ModuleJSON::CreateNewJSON(const char * path)
{
	JSON_Value* root_value = json_parse_file(path);
	const char* name = "josep";

	root_value = json_value_init_object();
	json_object_set_string(json_object(root_value), "name", name);
	json_serialize_to_file(root_value, path);
	name = json_object_get_string(json_object(root_value), "name");

	return root_value;
}

void ModuleJSON::print_commits_info(const char * username, const char * repo)
{
	JSON_Value *root_value;
	JSON_Array *commits;
	JSON_Object *commit;
	size_t i;

	char curl_command[512];
	char cleanup_command[256];
	char output_filename[] = "commits.json";

	/* it ain't pretty, but it's not a libcurl tutorial */
	sprintf(curl_command,
		"curl -s \"https://api.github.com/repos/%s/%s/commits\" > %s",
		username, repo, output_filename);
	sprintf(cleanup_command, "rm -f %s", output_filename);
	system(curl_command);

	/* parsing json and validating output */
	root_value = json_parse_file(output_filename);
	if (json_value_get_type(root_value) != JSONArray) {
		system(cleanup_command);
		return;
	}

	/* getting array from root value and printing commit info */
	commits = json_value_get_array(root_value);
	printf("%-10.10s %-10.10s %s\n", "Date", "SHA", "Author");
	for (i = 0; i < json_array_get_count(commits); i++) {
		commit = json_array_get_object(commits, i);
		printf("%.10s %.10s %s\n",
			json_object_dotget_string(commit, "commit.author.date"),
			json_object_get_string(commit, "sha"),
			json_object_dotget_string(commit, "commit.author.name"));
	}

	/* cleanup code */
	json_value_free(root_value);
	system(cleanup_command);
}

bool ModuleJSON::CleanUp()
{
	return false;
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

json_file::json_file(const char * path)
{
	this->path.append(path);
}

json_file::json_file()
{
}

json_file::~json_file()
{
}
json_file json_file::GetArray(const char * field, int index) const
{
	JSON_Array* array = json_object_get_array(object, field);
	if (array != nullptr)
		return json_file(json_array_get_object(array, index));

	return json_file((JSON_Object*) nullptr);
}

bool json_file::FindValue(const char * str, json_value_type type, int index) const
{
	bool ret = false;

	JSON_Value* val = GetValue(str, index);

	if (val != nullptr && json_value_get_type(val) == type)
		ret = true;

	return ret;

}

JSON_Value * json_file::GetValue(const char * field, int index) const
{
	//index are -1 by default
	if (index < 0)
		return json_object_get_value(object, field);

	JSON_Array* array = json_object_get_array(object, field);
	if (array != nullptr)
		return json_array_get_value(array, index);
	return nullptr;
}

const char * json_file::ReadString(const char * object_name)
{
	JSON_Value* root_value = json_parse_file(this->path.c_str());
	const char* name = json_object_get_string(json_object(root_value), object_name);
	return name;
}

int json_file::ReadInt(const char * int_name)
{
	JSON_Value* root_value = json_parse_file(this->path.c_str());
	JSON_Array* values = json_value_get_array(root_value);
	JSON_Object* object = json_array_get_object(values, 2);

	int number = json_object_dotget_number(object, int_name);// json_object_get_number(object, int_name);

	


	return number;

}

int json_file::GetInt(const char * set, int defaul, int id) const
{
	int ret = defaul;

	if (FindValue(set, json_value_type::JSONNumber, id))
		ret = json_object_dotget_number(object, set);

	return ret;
}
