#include "ModuleTimeManager.h"
#include "Application.h"


ModuleTimeManager::ModuleTimeManager(bool start_enabled):Module(start_enabled)
{
	game_timer.Stop();
	game_time = 0;
	time_scale = 1.0f;
	num_frames = 0;
}


ModuleTimeManager::~ModuleTimeManager()
{
}

void ModuleTimeManager::Play()
{
	EngineState state = App->state;



}

void ModuleTimeManager::Pause()
{
	EngineState state = App->state;
	
	if (state == playing)
	{
		state = paused;
		game_timer.Pause();
	}
}

void ModuleTimeManager::Stop()
{
	EngineState state = App->state;

	if (state == playing)
	{
		state = stopped;
		game_timer.Pause();
	}
}

update_status ModuleTimeManager::Update(float dt)
{
	if (App->state == playing)//Play
	{
		ingame_time = (SDL_GetTicks() - prev_dt)* time_scale;
		game_time += ingame_time;
		num_frames++;
	}
	else if (App->state == stopped)
	{
		game_time = 0;

	}
	else
	{
		ingame_time = 0;
	}

	prev_dt = SDL_GetTicks();
	return update_status::UPDATE_CONTINUE;

}
