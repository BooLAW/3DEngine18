#include "Profiler.h"

#define PLOT_MAX_LENGTH 20

bool Profiler::Draw(bool &active)
{
	ImGui::Begin("Profiler", &active);

	
	ImGui::Text("Initialization Timers");
	ImGui::Spacing();
	//Init timers
	ImGui::Text("Scene Init: %d ms", scene_init_time);
	ImGui::Spacing();
	ImGui::Text("Input Init: %d ms", input_init_time);
	ImGui::Spacing();
	ImGui::Text("Camera Init: %d ms", camera_init_time);
	ImGui::Spacing();
	ImGui::Text("Render Init: %d ms", render_init_time);
	ImGui::Spacing();
	ImGui::Text("UI Init: %d ms", ui_init_time);
	ImGui::Spacing();
	ImGui::Text("Audio Init: %d ms", audio_init_time);
	ImGui::Spacing();
	
	//--------------------------------------

	ImGui::Separator();
	ImGui::Text("Run time:");
	ImGui::Spacing();
	//Rund time performance for the ones needed(Input, Camera, Render)
	ImGui::PlotHistogram("",&input_buffer[0],input_buffer.size(),0,"Input",0.0,5.0,ImVec2(300,100));
	ImGui::PlotHistogram("", &render_buffer[0], render_buffer.size(), 0, "Render", 0.0, 5.0, ImVec2(300, 100));
	ImGui::PlotHistogram("", &camera_buffer[0], camera_buffer.size(), 0, "Camera", 0.0, 5.0, ImVec2(300, 100));
	ImGui::PlotHistogram("", &ui_buffer[0], ui_buffer.size(), 0, "UI", 0.0, 5.0, ImVec2(300, 100));

	ImGui::End();
	return true;
}

bool Profiler::SaveInitData(const char * module_name)
{
	if (module_name == "Scene")
		scene_init_time = scene_timer.Read();
	if (module_name == "Input")
		input_init_time = input_timer.Read();
	if (module_name == "Camera")
		camera_init_time = camera_timer.Read();
	if (module_name == "Render")
		render_init_time = render_timer.Read();
	if (module_name == "UI")
		ui_init_time = ui_timer.Read();
	if (module_name == "Audio")
		audio_init_time = audio_timer.Read();
	return true;
}

bool Profiler::SaveRunTimeData(const char * module_name)
{
	std::vector<float>* aux;
	//Get correct buffer
	if (module_name == "Scene") aux = &scene_buffer;
	if (module_name == "Input") aux = &input_buffer;
	if (module_name == "Camera") aux = &camera_buffer;
	if (module_name == "Render") aux = &render_buffer;
	if (module_name == "UI") aux = &ui_buffer;
	if (module_name == "Audio") aux = &audio_buffer;



	//erase oldest read
	if (aux->size() > PLOT_MAX_LENGTH)
		aux->erase(aux->begin());
	//add new read
	if (module_name == "Scene")
	{
		aux->push_back(scene_timer.Read()); 
		scene_timer.Start();
	}
	if (module_name == "Input")
	{
		aux->push_back(input_timer.Read());
		input_timer.Start();
	}
	if (module_name == "Camera")
	{
		aux->push_back(camera_timer.Read());
		camera_timer.Start();
	}	if (module_name == "Render")
	{
		aux->push_back(render_timer.Read());
		render_timer.Start();
	}	if (module_name == "UI")
	{
		aux->push_back(ui_timer.Read());
		ui_timer.Start();
	}	if (module_name == "Audio")
	{
		aux->push_back(audio_timer.Read());
		audio_timer.Start();
	}
	return true;
}

bool Profiler::StartTimer(const char * name)
{
	if (name == "Scene")
		scene_timer.Start();
	if (name == "Input")
		input_timer.Start();
	if (name == "Camera")
		camera_timer.Start();
	if (name == "Render")
		render_timer.Start();
	if (name == "UI")
		ui_timer.Start();
	if (name == "Audio")
		audio_timer.Start();
	return true;
}

bool Profiler::PauseTimer(const char * name)
{
	if (name == "Scene")
		scene_timer.Pause();
	if (name == "Input")
		input_timer.Pause();
	if (name == "Camera")
		camera_timer.Pause();
	if (name == "Render")
		render_timer.Pause();
	if (name == "UI")
		ui_timer.Pause();
	if (name == "Audio")
		audio_timer.Pause();
	return true;	
}

bool Profiler::ResumeTimer(const char * name)
{
	if (name == "Scene")
		scene_timer.Resume();
	if (name == "Input")
		input_timer.Resume();
	if (name == "Camera")
		camera_timer.Resume();
	if (name == "Render")
		render_timer.Resume();
	if (name == "UI")
		ui_timer.Resume();
	if (name == "Audio")
		audio_timer.Resume();
	return true;
}
