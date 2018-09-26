#pragma once
#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "MathGeoLib\MathGeoLib.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

class Application;
struct PhysBody3D;


class Module
{
private :
	bool enabled;

public:

	Module( bool start_enabled = true)
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}
	virtual void DrawModuleConfig() {}
	virtual void BroadcastEvent(SDL_Event & event) {}
	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}
	virtual bool Save(Document* config_w) { return true; }
	virtual bool Load(Document* config_r) { return true; }
};
extern Application* App;
