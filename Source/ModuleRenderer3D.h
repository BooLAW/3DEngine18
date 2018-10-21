#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#define MAX_LIGHTS 8
struct RenderAttributes{
	bool wireframe = false;
	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;
	bool texture = false;
	bool debug_draw_atribute = false;
};
class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D( bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	void DrawModuleConfig()override;
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	void SetUILights();
	void SetSceneLights();
	void CPUCapabilities();
	void UpdateAttributes(bool activate_sound = true);
	//DEBUG OPTIONS
	void SetDebugAttributes();
	void SetNormalAttributes();
	bool Save(Document& config_w, FileWriteStream& os);
	bool Load(Document* config_r);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	float3x3 NormalMatrix;
	float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	RenderAttributes attributes;
	//RENDERING FLAGS
	bool show_plane = true;
	bool debug_draw = false;
};