#include "PanelConsole.h"

PanelConsole::PanelConsole() :Panel("Console")
{
	width = 800;
	height = 150;
	pos_x = 250;
	pos_y = 800;
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
		text_buffer.clear();
	}
	if (show_debug_text)
		ImGui::TextUnformatted(text_buffer.begin());
	ImGui::End();
}

void PanelConsole::LogToConsole(const char * text)
{
	text_buffer.append(text);

}

void PanelConsole::ClearConsole()
{
	text_buffer.clear();
}
