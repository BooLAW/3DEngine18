#pragma once
#include "Module.h"
#include "Globals.h"
#include "Panel.h"

struct PhysBody3D;


class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	void rapidjsonexamplecode();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	int RandInt(int min, int max);
	float RandFloat(float min, float max);
	//void DisplayRandNumber(bool isActive);
	void Log(const std::string text);


};
