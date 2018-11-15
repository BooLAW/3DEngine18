#include "ModuleTimeManager.h"
#include "Timer.h"


ModuleTimeManager::ModuleTimeManager(bool start_enabled):Module(start_enabled)
{
	//game_timer.Stop();
	//game_time = 0;
	//time_scale = 1.0f;
	//num_frames = 0;
}


ModuleTimeManager::~ModuleTimeManager()
{
}

void ModuleTimeManager::Play()
{
}

void ModuleTimeManager::Pause()
{
}

void ModuleTimeManager::Stop()
{
}

update_status ModuleTimeManager::Update(float dt)
{
	//if (1)//Play
	//{
	//	ingame_time = (SDL_GetTicks() - prev_dt)* time_scale;
	//	game_time += ingame_time;
	//	num_frames++;
	//}
	//else//pause or stop
	//{
	//	ingame_time = 0;
	//}

	//prev_dt = SDL_GetTicks();
	return update_status::UPDATE_CONTINUE;

}
