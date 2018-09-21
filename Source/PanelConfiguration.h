#pragma once
#include "Panel.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "../Source/ImGui/imgui_internal.h"


class PanelConfiguration :
	public Panel
{
public:
	//Application
	//FPS Display

	float fps_log[32];

	//Milisecons Display

	float mms_log[32];


public:
	PanelConfiguration();
	virtual ~PanelConfiguration();
	void Draw()override;
	void Application();
	void Window();
	void Camera();
	void Input();
	void Render();
	void Hardware();


};

