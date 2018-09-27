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
	CONSOLE_LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		CONSOLE_LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
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
			CONSOLE_LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
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
	CONSOLE_LOG("Destroying SDL window and quitting all SDL systems");

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
		if (size_modified == true)
			Resize(width, height);
		if (ImGui::DragInt("Width", &width, 1, 1, 10000))
			size_modified = true;

		if (ImGui::DragInt("Height", &height, 1, 1, 10000))
			size_modified = true;


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

bool ModuleWindow::Save(Document& config_w, FileWriteStream& os)
{

	Document::AllocatorType& allocator = config_w.GetAllocator();

	Value test(kObjectType);


	test.AddMember("fullscreen", false, allocator);
	test.AddMember("width", width, allocator);
	test.AddMember("height", height, allocator);
	config_w.AddMember("window", test, allocator);

	return true;
}
bool ModuleWindow::Load(Document* config_r)
{

	Document ret;
	ret.Parse(App->loadBuf);
	ret.IsObject();
	width = ret["window"]["width"].GetInt();
	height = ret["window"]["height"].GetInt();
	fullscreen = ret["window"]["fullscreen"].GetBool();

	//screen_surface = SDL_GetWindowSurface(window);

	return true;
}
void ModuleWindow::Resize(int w, int h)
{
	SDL_SetWindowSize(window, w, h);
	width = w;
	height = h;
	size_modified = false;
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
