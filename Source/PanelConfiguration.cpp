#include "PanelConfiguration.h"



PanelConfiguration::PanelConfiguration():Panel("Configuration")
{
	width = 375;
	height = 800;
	pos_x = 900;
	pos_y = 20;
}


PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Draw()
{
	ImGui::Begin("Configuration", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing);	
	Application();
	ImGui::End();
}

void PanelConfiguration::Application()
{
	if (ImGui::CollapsingHeader("Application"))
	{

		ImGui::Checkbox("Vsync", &App->imgui->isVsyncActive);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::InputText("Organization", &charbuffer,200);
		ImGui::Separator();
		int arr_size = 7;
		static float arr[7] = { 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f };
		if (arr[0] != ImGui::GetIO().Framerate)
		{
			for (int i = 0; i < arr_size; i++)
			{
				arr[i + 1] = arr[i];
			}
			arr[0] = ImGui::GetIO().Framerate;

		}
		ImGui::PlotHistogram("Frame Rate", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));

	}
}

void PanelConfiguration::Hardware()
{

}
