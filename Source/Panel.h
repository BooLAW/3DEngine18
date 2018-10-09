#pragma once
#ifndef __PANEL__H__
#define __PANEL__H__
#include "Globals.h"
#include <string>
#include "ImGui/imgui.h"
#include "ImGui/imgui_dock.h"




class Panel
{
public:
	Panel(const char* name);
	virtual ~Panel();

	void Activate();
	bool IsActive();
	virtual void Draw();
public:
	bool active = true;
	int ini_width, ini_height, ini_pos_x, ini_pos_y;
	ImVec2 render_pos;
	ImVec2 render_size;
	const char* panel_name = nullptr;


};
#endif
