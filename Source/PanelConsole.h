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

	void LogToConsole(const char* text);
	void ClearConsole();
public:
	ImGuiTextBuffer text_buffer;
};
#endif
