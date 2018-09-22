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
		ImGui::InputText("App Name", TITLE, 32);
		ImGui::InputText("Organization", App->imgui->organization_name, 32);
		
		ImGui::SliderInt("Max FPS", &App->imgui->fps_slider, 0, 120);
		ImGui::Text("Limit Framerate: %i FPS", App->imgui->fps_slider);

		static int i = 32;
		if (fps_log[IM_ARRAYSIZE(fps_log)] != ImGui::GetIO().Framerate)
		{

			if (i >= 0)
			{
				fps_log[i - 1] = fps_log[i];
				i--;
			}
			else
			{
				i = IM_ARRAYSIZE(fps_log);
			}
			fps_log[IM_ARRAYSIZE(fps_log)-1] = ImGui::GetIO().Framerate;
		}

		static char tmp_string[4096];
		sprintf_s(tmp_string, 4096, "Framerate: %.2f", ImGui::GetIO().Framerate);
		ImGui::PlotHistogram("##framerate", fps_log, IM_ARRAYSIZE(fps_log), 0, tmp_string, 0.0f, 120.0f, ImVec2(310, 100));

		static int i2 = 32;
		float mss = (1000.0f / ImGui::GetIO().Framerate);
		if (mms_log[IM_ARRAYSIZE(mms_log)] != mss)
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
			mms_log[IM_ARRAYSIZE(mms_log)] = mss;
		}

		static char tmp_string2[4096];
		sprintf_s(tmp_string2, 4096, "Miliseconds: %.3f", mss);
		ImGui::PlotHistogram("##miliseconds", mms_log, IM_ARRAYSIZE(mms_log), 0, tmp_string2, 0.0f, 120.0f, ImVec2(310, 100));

	}
}

void PanelConfiguration::Window()
{

}

void PanelConfiguration::Hardware()
{

}
