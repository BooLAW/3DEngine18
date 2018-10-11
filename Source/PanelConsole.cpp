#include "PanelConsole.h"

PanelConsole::PanelConsole() :Panel("Console")
{
	ini_pos_x = 250;
	ini_pos_y = 800;
	ini_width = 800;
	ini_height = 150;
	active = true;
}
PanelConsole::~PanelConsole()
{

}

void PanelConsole::Draw()
{
	if (ImGui::BeginDock("Console", &active))
	{
		ImGui::SameLine();
		if (ImGui::Button("DEBUG"))
		{
			show_debug_text = !show_debug_text;
		}
		ImGui::SameLine();
		if (ImGui::Button("WARNINGS"))
		{
			show_warning_text = !show_warning_text;
		}
		ImGui::SameLine();
		if (ImGui::Button("ERRORS"))
		{
			show_err_text = !show_err_text;
		}
		ImGui::SameLine();
		if (ImGui::Button("INFO"))
		{
			show_info_text = !show_info_text;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			ClearConsole();
		}
		ImGui::Separator();
		ImGui::BeginChild("ConsoleText");
		if (show_debug_text)
			for (int i = 0; i < text_debug_buffer.size(); i++)
			{
				ImGui::Text(text_debug_buffer[i].c_str());
			}
		if (show_warning_text)
			for (int i = 0; i < text_warning_buffer.size(); i++)
			{
				ImGui::Text(text_warning_buffer[i].c_str());
			}
		
		ImGui::SetScrollY(ImGui::GetScrollMaxY());
		render_pos = ImGui::GetWindowPos();
		render_size = ImGui::GetWindowSize();
		ImGui::EndChild();
	}

	ImGui::EndDock();

}

void PanelConsole::LogToConsole(const std::string text,int type)
{
	switch (type)
	{
	case 0:
		text_debug_buffer.push_back("-DEB:: " + text);
	case 1:
		text_warning_buffer.push_back("-WAR:: "+text);
	case 2:
		text_err_buffer.push_back("-ERR:: " + text);
	case 3:
		text_info_buffer.push_back("-INF:: " +text);
	}

}

void PanelConsole::ClearConsole()
{
	text_debug_buffer.clear();
	text_warning_buffer.clear();
	text_err_buffer.clear();
	text_info_buffer.clear();

}
