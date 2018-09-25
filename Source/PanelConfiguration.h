#pragma once
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

public:
	PanelConfiguration();
	virtual ~PanelConfiguration();
	void Draw()override;
	void Application();



};

