#include "PanelConsole.h"

PanelConsole::PanelConsole() :Panel("Console")
{
	width = 800;
	height = 150;
	pos_x = 250;
	pos_y = 800;
	active = true;
}
PanelConsole::~PanelConsole()
{

}

void PanelConsole::Draw()
{
	ImGui::Begin("Console", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing);

	if(ImGui::Button("Debug"))
	{
		show_debug_text = !show_debug_text;
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{
		text_debug_buffer.clear();
	}
	if (show_debug_text)
		for (int i = 0; i < text_debug_buffer.size(); i++)
		{
			ImGui::Text(text_debug_buffer[i].c_str());
		}

	ImGui::SetScrollY(ImGui::GetScrollMaxY());
	ImGui::End();	
}

void PanelConsole::LogToConsole(const std::string text)
{
	text_debug_buffer.push_back(text);

}

void PanelConsole::ClearConsole()
{
	text_debug_buffer.clear();
}
