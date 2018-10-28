#ifndef __PANEL_CONFIG_H__
#define __PANEL_CONFIG_H__


#include "Panel.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "../Source/ImGui/imgui_internal.h"


class PanelConfiguration :
	public Panel
{
public:
	float fps_log[32];
	float mms_log[32];
	float mem_log[32];
	static char app_name_input;
	static char app_org_input;
	int final_mem = 0;
	int last_mem = 0;
public:
	PanelConfiguration();
	virtual ~PanelConfiguration();
	void Draw()override;
	void Application();
};
#endif
