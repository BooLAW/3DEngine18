#pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"
#include "Panel.h"
#include "PanelConsole.h"

struct PhysBody3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	int RandInt(int min, int max);
	float RandFloat(float min, float max);
	void DisplayRandNumber(bool isActive);
	void Log(char* text);
public:
	std::vector<Panel*> panels;

public:
	PanelConsole* console = nullptr;

};
