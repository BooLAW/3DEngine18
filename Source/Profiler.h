#ifndef __PROFILER_H__
#define __PROFILER_H__

#include "Timer.h"
#include "ModuleSceneImGui.h"

class Profiler {
public:
	Profiler() {};
	~Profiler() {};

	bool Draw(bool &active);
	bool SaveInitData(const char* module_name);
	bool SaveRunTimeData(const char* module_name);

	bool StartTimer(const char* module_name);
	bool PauseTimer(const char* module_name);
	bool ResumeTimer(const char* module_name);
public:
	//timers
	Timer scene_timer;
	Timer input_timer;
	Timer camera_timer;
	Timer render_timer;
	Timer ui_timer;
	Timer audio_timer;
	
	//start time
	int scene_init_time;
	int input_init_time;
	int camera_init_time;
	int render_init_time;
	int ui_init_time;
	int audio_init_time;
	//buffers
	std::vector<float> scene_buffer;
	std::vector<float> input_buffer;
	std::vector<float> camera_buffer;
	std::vector<float> render_buffer;
	std::vector<float> ui_buffer;
	std::vector<float> audio_buffer;


};
#endif