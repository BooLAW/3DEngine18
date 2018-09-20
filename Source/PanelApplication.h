#pragma once
#include "Panel.h"
#include "ImGui/imgui.h"
#include "ModuleSceneImGui.h"
#include "Application.h"
#include "../Source/ImGui/imgui_internal.h"

class PanelApplication :
	public Panel
{

public:
	class ImGuiWindow;
	struct IMGUI_API ImGuiWindow;

	PanelApplication();
	virtual ~PanelApplication();
	void Draw()override;

};

