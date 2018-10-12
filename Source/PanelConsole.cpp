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
		for (int i = 0; i < text_buffer.size(); i++)
		{	
			if(CheckDisplayFlag(text_buffer[i]))
				ImGui::TextColored(GetColorFromLogType(text_buffer[i]), text_buffer[i].text.c_str());
		}
		

		
		ImGui::SetScrollY(ImGui::GetScrollMaxY());
		render_pos = ImGui::GetWindowPos();
		render_size = ImGui::GetWindowSize();
		ImGui::EndChild();
	}

	ImGui::EndDock();

}

bool PanelConsole::CheckDisplayFlag(typeLog log)
{
	switch (log.type)
	{
	case 0:
	{
		if (show_debug_text)
			return true;
	}
	case 1:
	{
		if (show_warning_text)
			return true;
	}
	case 2:
	{
		if (show_err_text)
			return true;
	}
	case 3:
	{
		if (show_info_text)
			return true;
	}
	}
	return false;
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
		//WHITE
		myColor.x = 1;
		myColor.y = 1;
		myColor.z = 1;
		myColor.w = 1;


		return myColor;
	}
		
	case 1:
	{
		//YELLOW
		myColor.x = 1;
		myColor.y = 1;
		myColor.z = 0;
		myColor.w = 1;


		return myColor;
	}
	case 2:
	{
		//RED
		myColor.x = 1;
		myColor.y = 0;
		myColor.z = 0;
		myColor.w = 1;



		return myColor;
	}
	case 3:
	{
		//BLUE
		myColor.x = 0;
		myColor.y = 0;
		myColor.z = 1;
		myColor.w = 1;
	


		return myColor;
	}
	}
}
