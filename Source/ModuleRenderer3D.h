#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "Primitive.h"

#define MAX_LIGHTS 8
struct RenderAttributes{
	bool wireframe = false;
	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;
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
	void UpdateAttributes();
	//DEBUG OPTIONS
	void SetDebugAttributes();
	void SetNormalAttributes();
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