#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "SDL/include/SDL_version.h"
#include "Primitive.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment(lib, "Glew/libx86/glew32.lib")
struct SDL_version;
ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module( start_enabled)
{
	
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	CONSOLE_LOG("Creating 3D Renderer context");
	App->profiler.StartTimer("Render");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		CONSOLE_LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	//Init GLEW
	glewInit();
	//Info LOG
	CONSOLE_LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	CONSOLE_LOG("Vendor: %s", glGetString(GL_VENDOR));
	CONSOLE_LOG("Renderer: %s", glGetString(GL_RENDERER));
	CONSOLE_LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	CONSOLE_LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	if(ret == true)
	{
		//Reset Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Reset Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		SetSceneLights();
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	App->profiler.SaveInitData("Render");
	return ret;
}

void ModuleRenderer3D::DrawModuleConfig()
{
	if (ImGui::CollapsingHeader("Renderer"))
	{
		
		SDL_version compiler;
		SDL_VERSION(&compiler);
		
		ImGui::Text("SDL Version:"); ImGui::SameLine(0, 10);
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d.%d.%d", compiler.major,compiler.minor,compiler.patch);

		ImGui::Text("CPUs: "); ImGui::SameLine(0, 5);
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i (Cache: %ikb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

		ImGui::Text("System RAM: "); ImGui::SameLine(0, 10);
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", SDL_GetSystemRAM());

		bool attribute_modified = false;
		if (ImGui::Checkbox("Wireframe", &attributes.wireframe)) attribute_modified = true;
		if(ImGui::Checkbox("DepthTest", &attributes.depth_test))attribute_modified = true;
		if(ImGui::Checkbox("CullTest", &attributes.cull_face))attribute_modified = true;
		if(ImGui::Checkbox("Lighting", &attributes.lighting))attribute_modified = true;
		if(ImGui::Checkbox("Color Material", &attributes.color_material))attribute_modified = true;

		if (attribute_modified)
			UpdateAttributes();
		CPUCapabilities();
	}

}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	App->profiler.StartTimer("Render");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	Color c = { 0,0,0,1 };
	glClearColor(c.r, c.g, c.b, c.a); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW); 
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	App->profiler.SaveRunTimeData("Render");

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	if (attributes.wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//Base Plane
	if (show_plane == true)
	{
		PPlane base_plane(0, 1, 0, 0);
		base_plane.axis = true;	
		base_plane.Render();
	}
	//PCube my_cube(5, 5, 5);
	//my_cube.Render();
	//Draw Scene
	//App->scene_intro->Update(dt);
	//Debug Draw
	if (debug_draw == true)
	{
		SetDebugAttributes();
		//DRAW FUTURE SCENE GAMEOBJECTS
		//App->DebugDraw();
		SetNormalAttributes();
	}

	App->imgui->DrawImGui();
	
	SetSceneLights();
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	CONSOLE_LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	mat4x4 aux;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = ProjectionMatrix.perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::SetUILights()
{
	GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

	GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

	GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
}

void ModuleRenderer3D::SetSceneLights()
{
	GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

	lights[0].ref = GL_LIGHT0;
	lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
	lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
	lights[0].SetPos(0.0f, 0.0f, 2.5f);
	lights[0].Init();

	GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

	GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

	UpdateAttributes();
	
	lights[0].Active(true);
}

void ModuleRenderer3D::CPUCapabilities()
{
	char* rdtsc = nullptr;
	char* mmx = nullptr;
	char* sse = nullptr;
	char* sse2 = nullptr;
	char* sse3 = nullptr;
	char* sse41 = nullptr;
	char* sse42 = nullptr;
	char* avx = nullptr;

	std::vector<char*> cpu_title;
	if (SDL_HasRDTSC())
	{
		char* rdtsc = "RDTSC";
		cpu_title.push_back(rdtsc);
	}
	if (SDL_HasMMX())
	{
		char* mmx = "MMX";
		cpu_title.push_back(mmx);
	}
	if (SDL_HasSSE())
	{
		char* sse = "SSE";
		cpu_title.push_back(sse);
	}
	if (SDL_HasSSE2())
	{
		char* sse2 = "SSE2";
		cpu_title.push_back(sse2);
	}
	if (SDL_HasSSE3())
	{
		char* sse3 = "SSE3";
		cpu_title.push_back(sse3);
	}
	if (SDL_HasSSE41())
	{
		char* sse41 = "SSE41";
		cpu_title.push_back(sse41);
	}
	if (SDL_HasSSE42())
	{
		sse42 = "SSE42";
		cpu_title.push_back(sse42);
	}
	if (SDL_HasAVX())
	{
		avx = "AVX";
		cpu_title.push_back(avx);
	}

	ImGui::Text("Caps:"); ImGui::SameLine(0, 5);
	for (int i = 0; i < cpu_title.size(); i++)
	{
		if (i == 0)
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", cpu_title[i]);
			ImGui::SameLine(0, 0);
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), ", %s", cpu_title[i]);
			ImGui::SameLine(0, 0);
		}
	}
}

void ModuleRenderer3D::UpdateAttributes()
{
	//wireframe
	if (attributes.wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else 		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//color_material
	if (attributes.color_material)
		glEnable(GL_COLOR_MATERIAL);
	else
		glDisable(GL_COLOR_MATERIAL);
	//depth test
	if (attributes.depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	//lighting
	if (attributes.lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
	//cull test
	if (attributes.cull_face)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

}

void ModuleRenderer3D::SetDebugAttributes()
{
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	glColor4f(1.f, 1.f, 1.f, 1.f);
}

void ModuleRenderer3D::SetNormalAttributes()
{
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

bool ModuleRenderer3D::Save(Document& testconfig_w, FileWriteStream& os)
{
	Document::AllocatorType& allocator = testconfig_w.GetAllocator();

	Value app(kObjectType);
	
	app.AddMember("Wireframe", attributes.wireframe, allocator);
	app.AddMember("DepthTest", attributes.depth_test, allocator);
	app.AddMember("CullTest", attributes.cull_face, allocator);
	app.AddMember("Lighting", attributes.lighting, allocator);
	app.AddMember("Color Material", attributes.color_material, allocator);
	testconfig_w.AddMember("renderer", app, allocator);
	
	return true;
}

bool ModuleRenderer3D::Load(Document* testconfig_r)
{
	Document ret;
	ret.Parse(App->loadBuf);
	ret.IsObject();
	attributes.wireframe = ret["renderer"]["Wireframe"].GetBool();
	attributes.depth_test = ret["renderer"]["DepthTest"].GetBool();
	attributes.cull_face = ret["renderer"]["CullTest"].GetBool();
	attributes.lighting = ret["renderer"]["Lighting"].GetBool();
	attributes.color_material = ret["renderer"]["Color Material"].GetBool();

	return true;
}

