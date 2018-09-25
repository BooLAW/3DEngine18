#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "SDL/include/SDL_version.h"

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
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		CONSOLE_LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		
		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
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

		CPUCapabilities();
	}

}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());


	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
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
	float4x4 aux;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix =  aux.perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf((float*)ProjectionMatrix.v);

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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	lights[0].Active(true);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
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

