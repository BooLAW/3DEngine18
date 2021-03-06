#include "PanelConfiguration.h"
#include "stdio.h"
#include "mmgr/mmgr.h"
#include "SDL/include/SDL_video.h"
#include "Application.h"





PanelConfiguration::PanelConfiguration():Panel("Configuration")
{
	ini_pos_x = 800;
	ini_pos_y = 20;
	ini_width = 475;
	ini_height = 800;
	render_pos = { 800,20 };
	render_size = { 800,20 };
}


PanelConfiguration::~PanelConfiguration()
{

}

void PanelConfiguration::Draw()
{
	if(ImGui::BeginDock("Configuration", &active))
	{
		render_pos = ImGui::GetWindowPos();
		render_size = ImGui::GetWindowSize();
		Application();
		for (int i = 0; App->GetModule(i) != nullptr; i++)
		{
			App->GetModule(i)->DrawModuleConfig();
		}
	}
	ImGui::EndDock();

}

void PanelConfiguration::Application()
{
	if (ImGui::CollapsingHeader("Application"))
	{
		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->config_tick_arr[0]);
		ImGui::Checkbox("Vsync", &App->imgui->isVsyncActive);
		if (App->imgui->isVsyncActive == true)
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->config_tick_arr[1]);
			App->audio->config_tick_arr[2] = FALSEBOOL;
		}
		else
		{			
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->config_tick_arr[2]);
			App->audio->config_tick_arr[1] = FALSEBOOL;	
		}
		static char app_name[128] = TITLE;
		static char org_name[128] = ORGANIZATION;
		ImGui::Text("Application Delta Time: %f", App->dt);
		ImGui::Text("Application Sleeping Time: %f", App->sleeping_time);
		
		ImGui::InputText("App Name", app_name, IM_ARRAYSIZE(app_name));
		ImGui::InputText("Organization", org_name, IM_ARRAYSIZE(org_name));

		std::string window_title;
		window_title.append(app_name);
		window_title.append(" - ");
		window_title.append(org_name);


		App->window->SetTitle(window_title.c_str());
		if (ImGui::SliderInt("Max FPS", &App->imgui->fps_slider, 0, 120))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->config_tick_arr[3]);
			App->audio->config_tick_arr[3] = FALSEBOOL;
		}
		ImGui::Text("Limit Framerate: %i FPS", App->imgui->fps_slider);

		static int i = 32;
		if (fps_log[IM_ARRAYSIZE(fps_log)] != ImGui::GetIO().Framerate)
		{

			if (i > 0)
			{
				fps_log[i - 1] = fps_log[i];
				i--;
			}
			else
			{
				i = IM_ARRAYSIZE(fps_log);
			}
			fps_log[IM_ARRAYSIZE(fps_log) - 1] = ImGui::GetIO().Framerate;
		}

		static char tmp_string[4096];
		sprintf_s(tmp_string, 4096, "Framerate: %.2f", ImGui::GetIO().Framerate);
		ImGui::PlotHistogram("##framerate", fps_log, IM_ARRAYSIZE(fps_log), 0, tmp_string, 0.0f, 120.0f, ImVec2(310, 100));

		static int i2 = 32;
		App->mms_fps = (1000.0f / ImGui::GetIO().Framerate);
		if (mms_log[IM_ARRAYSIZE(mms_log)] != App->mms_fps)
		{
			if (i2 > 0)
			{
				mms_log[i2 - 1] = mms_log[i2];
				i2--;
			}
			else
			{
				i2 = IM_ARRAYSIZE(mms_log);
			}
			mms_log[IM_ARRAYSIZE(mms_log)-1] = App->mms_fps;
		}

		static char tmp_string2[4096];
		sprintf_s(tmp_string2, 4096, "Miliseconds: %.3f", App->mms_fps);
		ImGui::PlotHistogram("##miliseconds", mms_log, IM_ARRAYSIZE(mms_log), 0, tmp_string2, 0.0f, 120.0f, ImVec2(310, 100));

		static int i3 = 32;
		//float mss = (1000.0f / ImGui::GetIO().Framerate);
		sMStats stats = m_getMemoryStatistics();

		static char tmp_string3[4096];
		final_mem = stats.peakActualMemory;
		final_mem = final_mem / 102400;

		if (mem_log[IM_ARRAYSIZE(mem_log)] != final_mem)
		{			
			if (i3 > 0)
			{
				mem_log[i3 - 1] = mem_log[i3];
				i3--;
			}
			else
			{
				i3 = IM_ARRAYSIZE(mem_log);
			}

			
			mem_log[IM_ARRAYSIZE(mem_log) - 1] = (float)final_mem;
			
			
		}
		sprintf_s(tmp_string3, 4096, "Memory: %f", (float)final_mem);

		ImGui::PlotHistogram("##memoryalloc", mem_log, IM_ARRAYSIZE(mem_log), 0, tmp_string3, 0.0f, 8000.0f, ImVec2(310, 100));
		last_mem = stats.peakActualMemory;
		ImGui::Text("Total Reported Mem: %u", stats.totalReportedMemory);
		ImGui::Text("Total Actual Mem: %u", stats.totalActualMemory);
		ImGui::Text("Peak Reported Mem: %u", stats.peakReportedMemory);
		ImGui::Text("Peak Actual Mem: %u", stats.peakActualMemory);
		ImGui::Text("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
		ImGui::Text("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
		ImGui::Text("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
		ImGui::Text("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
		ImGui::Text("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);
	}
	else
		App->audio->config_tick_arr[0] = FALSEBOOL;
}

