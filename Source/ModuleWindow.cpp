#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module( start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		 width = SCREEN_WIDTH * SCREEN_SIZE;
		 height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::DrawModuleConfig()
{
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	if (ImGui::CollapsingHeader("Window"))
	{

		ImGui::Text("Size configuration:");

		ImGui::SliderInt("Width", &width, 600, DM.w);
		ImGui::SliderInt("Height", &height, 600, DM.h);

		SDL_SetWindowSize(window, width, height);

		if (SDL_UpdateWindowSurface(window)) //(?)
			screen_surface = SDL_GetWindowSurface(window);

		if (ImGui::Button("Center Window"))
		{
			SDL_SetWindowPosition(window, DM.w / 2 - width / 2, DM.h / 2 - height / 2);
		}
		ImGui::Separator();
		// Auto adjustments -----------------------------

		ImGui::Text("Auto adjustments: ");

		ImGui::Checkbox("Fullscreen", &fullscreen); ImGui::SameLine();
		ImGui::Checkbox("Borderless", &borderless);

		if (fullscreen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		else
			SDL_SetWindowFullscreen(window, 0);

		if (borderless)
			SDL_SetWindowBordered(window, SDL_bool(false));
		else
			SDL_SetWindowBordered(window, SDL_bool(true));
		ImGui::Separator();
		// ------------------------------------------
	}
}

void ModuleWindow::BroadcastEvent(SDL_Event & event)
{
	switch (event.window.event)
	{
	case SDL_WINDOWEVENT_RESIZED:
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	{
		if (event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			width = event.window.data1;
			height = event.window.data2;

			App->renderer3D->OnResize(width, height);
		}

	}
	break;
	}
}
