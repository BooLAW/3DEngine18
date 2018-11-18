#include "ModuleTimeManager.h"
#include "LoadManager.h"
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

	if (state == paused)
	{
		state = playing;
		game_timer.Resume();
	}
	if (state == stopped)
	{
		App->scene_intro->SaveScene(App->scene_intro->go_list);
		state = playing;
		game_timer.Start();
	}

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
		App->scene_intro->LoadScene(".\\Assets\\Scenes\\scene1.json");
		state = stopped;
		game_timer.Stop();
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
