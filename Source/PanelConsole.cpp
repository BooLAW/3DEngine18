#include "PanelConsole.h"
#include "Globals.h"
#include "Color.h"

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
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 1, 0.3));
		if (ImGui::Button("DEBUG"))
		{
			show_debug_text = !show_debug_text;
		}
		ImGui::PopStyleColor(1);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1,1,0,0.3));

		if (ImGui::Button("WARNINGS"))
		{
			show_warning_text = !show_warning_text;
		}
		ImGui::PopStyleColor(1);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0.3));
		if (ImGui::Button("ERRORS"))
		{
			show_err_text = !show_err_text;
		}
		ImGui::PopStyleColor(1);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.3));
		if (ImGui::Button("INFO"))
		{
			show_info_text = !show_info_text;
		}
		ImGui::SameLine();
		ImGui::PopStyleColor(1);

		if (ImGui::Button("Clear"))
		{
			ClearConsole();
		}

		ImGui::Separator();

		ImGui::BeginChild("ConsoleText");
		for (int i = 0; i < text_buffer.size(); i++)
		{	
			to_show = CheckDisplayFlag(text_buffer[i].type);
			if (to_show) {
				ImGui::TextColored(GetColorFromLogType(text_buffer[i]), text_buffer[i].text.c_str());

			}
		}
		

		
		ImGui::SetScrollY(ImGui::GetScrollMaxY());
		render_pos = ImGui::GetWindowPos();
		render_size = ImGui::GetWindowSize();
		ImGui::EndChild();
	}

	ImGui::EndDock();

}

bool PanelConsole::CheckDisplayFlag(int type)
{
	bool ret = false;

	switch (type)
	{
		case 0:
		{
			if (show_debug_text)
				ret =  true;
			break;
		}
		case 1:
		{
			if (show_warning_text)
				ret=  true;
			break;
		}
		case 2:
		{
			if (show_err_text)
				ret =  true;
			break;
		}
		case 3:
		{
			if (show_info_text)
				ret =  true;
			break;
		}
		default:
			break;
	}
	return ret;
}

void PanelConsole::LogToConsole(const std::string text,int type)
{
	typeLog aux;
	aux.text = text;
	aux.type = type;
	
	text_buffer.push_back(aux);
	

}

void PanelConsole::ClearConsole()
{
	text_buffer.clear();

}

const ImVec4 PanelConsole::GetColorFromLogType(typeLog log)
{
	ImVec4 myColor;

	switch (log.type)
	{
	case 0:
	{

		return Blue;
	}
		
	case 1:
	{

		return Yellow;
	}
	case 2:
	{


		return Red;
	}
	case 3:
	{

		return White;
	}
	}
}
