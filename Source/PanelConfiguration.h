#pragma once
#include "Panel.h"
#include "ImGui/imgui.h"

class PanelConfiguration :
	public Panel
{
public:
	PanelConfiguration();
	virtual ~PanelConfiguration();
	void Draw()override;
};

