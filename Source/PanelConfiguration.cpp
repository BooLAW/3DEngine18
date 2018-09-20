#include "PanelConfiguration.h"
#include "stdio.h"



PanelConfiguration::PanelConfiguration():Panel("Configuration")
{
	width = 475;
	height = 800;
	pos_x = 800;
	pos_y = 20;
}


PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Draw()
{
	ImGui::Begin("Configuration", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing);	
	Application();
	for (int i = 0; App->GetModule(i) != nullptr; i++)
	{
		App->GetModule(i)->DrawModuleConfig();
	}
	ImGui::End();
}

void PanelConfiguration::Application()
{
	if (ImGui::CollapsingHeader("Application"))
	{
		ImGui::Checkbox("Vsync", &App->imgui->isVsyncActive);
		if (App->imgui->isVsyncActive == true)
		{
			//Activate Vsync
			if (SDL_GL_SetSwapInterval(1))
				LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Deactivate Vsync
			if (SDL_GL_SetSwapInterval(0))
				LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::InputText("App Name", "Living Worlds", 14);
		ImGui::InputText("Organization", "UPC CITM", 9);
		static int fps_slider = 60;
		ImGui::SliderInt("Max FPS", &fps_slider, 0, 120);
		ImGui::Text("Limit Framerate: %i FPS", fps_slider);

		int fps_log_size = 7;
		if (fps_log[0] != ImGui::GetIO().Framerate)
		{
			fps_log[0] = ImGui::GetIO().Framerate;
			for (int i = 0; i < fps_log_size; i++)
			{
				fps_log[i + 1] = fps_log[i];
			}
		}

		static char tmp_string[4096];
		sprintf_s(tmp_string, 4096, "Framerate: %.1f", ImGui::GetIO().Framerate);
		ImGui::PlotHistogram("##framerate", fps_log, IM_ARRAYSIZE(fps_log), 0, tmp_string, 0.0f, 120.0f, ImVec2(310, 100));

		int mms_log_size = 7;
		if (mms_log[0] != (1000.0f / ImGui::GetIO().Framerate))
		{
			mms_log[0] = (1000.0f / ImGui::GetIO().Framerate);
			for (int i = 0; i < mms_log_size; i++)
			{
				mms_log[i + 1] = mms_log[i];
			}
		}
		static char tmp_string2[4096];
		sprintf_s(tmp_string2, 4096, "Miliseconds: %.3f", 1000 / ImGui::GetIO().Framerate);
		ImGui::PlotHistogram("##miliseconds", mms_log, IM_ARRAYSIZE(mms_log), 0, tmp_string2, 0.0f, 120.0f, ImVec2(310, 100));

	}
}

void PanelConfiguration::Window()
{

}

void PanelConfiguration::Hardware()
{

}
