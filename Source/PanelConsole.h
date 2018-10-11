#pragma once
#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__
#include "Panel.h"

class PanelConsole : public Panel
{
public:
	PanelConsole();
	virtual ~PanelConsole();


	void Draw()override;

	void LogToConsole(const std::string text,int type);
	void ClearConsole();

public:
	std::vector<std::string> text_debug_buffer;
	std::vector<std::string> text_warning_buffer;
	std::vector<std::string> text_err_buffer;
	std::vector<std::string> text_info_buffer;

	bool show_debug_text = true;
	bool show_warning_text = true;
	bool show_err_text = true;
	bool show_info_text = true;


};
#endif
