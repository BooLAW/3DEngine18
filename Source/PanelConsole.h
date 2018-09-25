#pragma once
#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__
#include "Panel.h"
#include "ImGui/imgui.h"
class PanelConsole : public Panel
{
public:
	PanelConsole();
	virtual ~PanelConsole();


	void Draw()override;

	void LogToConsole(const std::string text);
	void ClearConsole();
	bool show_debug_text = true;
public:
	std::vector<std::string> text_debug_buffer;
};
#endif
