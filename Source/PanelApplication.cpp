#include "PanelApplication.h"



PanelApplication::PanelApplication():Panel("Application")
{
	width = 500;
	height = 1400;
	pos_x = 500;
	pos_y = 200;
}


PanelApplication::~PanelApplication()
{
}

void PanelApplication::Draw()
{
	ImGui::Begin("Application", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing);

	ImGui::Checkbox("Vsync", &App->imgui->isVsyncActive);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Framerate AVG: %.1f FPS", ImGui::GetIO().Framerate);
	ImGui::LabelText("UPC","Organization");
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


	ImGui::End();
}
