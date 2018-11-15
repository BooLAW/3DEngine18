#ifndef __MODULE_TIME_MANAGER_H__
#define __MODULE_TIME_MANAGER_H__

#include "Module.h"
#include "Globals.h"

class ModuleTimeManager : public Module
{
public:
	ModuleTimeManager(bool start_enabled = true);
	virtual ~ModuleTimeManager();
	//Functions to call when cliccking Buttons
	void Play();
	void Pause();
	void Stop();
	void Step();//Wait for the other 3 to work

	update_status Update(float dt) override;

public:
	/*Timer real_timer;
	Timer game_timer;

	uint num_frames;

	Uint32 game_time;
	Uint32 prev_dt;

	float ingame_time;

	float time_scale;*/
};
#endif
