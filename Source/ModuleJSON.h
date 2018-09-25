#ifndef _JSON_H_
#define _JSON_H_

#include "Module.h"
#include "Parson\parson.h"
#include<string>
#include<list>
class root_value;
class json_file
	 {
	 public:
		 json_file(JSON_Value* value, JSON_Object* object, const char* path);
		 json_file(JSON_Object* Entry);
		 json_file(const char* path);
		 json_file();
		 ~json_file();

		 void Save(); 
		 void CleanUp();
		 void Delete();
		 const char* ReadString(const char* object_name);
		 int ReadInt(const char* int_name);
		 int GetInt(const char* set, int defaul, int id = -1)const;
		 json_file json_file::GetArray(const char * field, int index) const;
		 bool json_file::FindValue(const char * str, json_value_type type, int index)const;
		 JSON_Value * GetValue(const char * field, int index) const;

	 public:
		JSON_Value* value = nullptr;
		JSON_Object* object = nullptr;
		JSON_Object* root = nullptr;
		JSON_Array* array = nullptr;
		std::string path;
};


class ModuleJSON : public Module
{
public:
	ModuleJSON(bool enabled);
	~ModuleJSON();
	bool Awake();
	JSON_Value* CreateNewJSON(const char* path);
	void print_commits_info(const char*username, const char *repository);
	bool CleanUp();
private:
	std::list<json_file*> j_files;
};


#endif // !_JSON_H_
