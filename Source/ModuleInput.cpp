#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput( bool start_enabled) : Module( start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

bool ModuleInput::Init()
{
	CONSOLE_LOG_INFO("Init SDL input event system");
	App->profiler.StartTimer("Input");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		CONSOLE_LOG_ERROR("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	App->profiler.SaveInitData("Input");
	return ret;
}

update_status ModuleInput::PreUpdate(float dt)
{
	App->profiler.StartTimer("Input");
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		App->imgui->ManageInput(&e);
		switch(e.type)
		{
			case SDL_DROPFILE:
			{
				std::string file = e.drop.file;
				App->loading_manager->Load(file.c_str());
			}
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
					App->window->size_modified = true;
					App->window->width = e.window.data1;
					App->window->height = e.window.data2;
				}
			}
			break;
		}
	}
	App->profiler.SaveRunTimeData("Input");
	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	CONSOLE_LOG_INFO("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::DrawModuleConfig()
{
	ImGuiIO& io = ImGui::GetIO();

	io.WantCaptureMouse;
	io.WantCaptureKeyboard;
	if (ImGui::CollapsingHeader("Input"))
	{
		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->input_tick_arr[0]);
		ImGui::Text("Mouse Position:");
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "X: %d", App->input->mouse_x);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Y: %d", App->input->mouse_y);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Mouse Wheel: %d", App->input->mouse_z);


		ImGui::Text("Mouse Motion:");
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "X: %d", App->input->mouse_x_motion);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Y: %d", App->input->mouse_y_motion);


		ImGui::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d (%.02f secs)", i, io.KeysDownDuration[i]); }
		ImGui::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyPressed(i)) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", i); }
		ImGui::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyReleased(i)) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", i); }
		ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "");
	}
	else
		App->audio->input_tick_arr[0] = FALSEBOOL;

}
