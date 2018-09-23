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
		 json_file();
		 ~json_file();

		 void Save(); 
		 void CleanUp();
		 void Delete();
	 private:
		 bool FindValue(const char* str, json_value_type type,int index)const;
		 JSON_Value * GetValue(const char * field, int index) const;

	 private:
		JSON_Value* value = nullptr;
		JSON_Object* object = nullptr;
		JSON_Object* root = nullptr;
		JSON_Array* array = nullptr;
		std::string path;
};


class JSON : public Module
{
public:
	JSON(bool enabled);
	~JSON();
	bool Awake();
	JSON_Value* LoadJSONFile(const char* path);
	json_file* CreateNewJSON(const char* path);
	json_file* GetFile(const char* name);
	bool CleanUp();
private:
	std::list<json_file*> j_files;
};


#endif // !_JSON_H_
