#pragma once
#include "Panel.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "../Source/ImGui/imgui_internal.h"

class PanelConfiguration :
	public Panel
{
public:
	char charbuffer;

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

