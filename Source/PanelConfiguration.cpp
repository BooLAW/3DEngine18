#include "PanelConfiguration.h"



PanelConfiguration::PanelConfiguration():Panel("Configuration")
{
	width = 200;
	height = 800;
	pos_x = 1100;
	pos_y = 20;
}


PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Draw()
{
	ImGui::Begin("Configuratuion", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing);	
	ImGui::End();
}
