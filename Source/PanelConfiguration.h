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
	float fps_log[7] = { 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f };
	float mms_log[7] = { 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f };

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

