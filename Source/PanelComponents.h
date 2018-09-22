#pragma once
#ifndef __PANELCOMPONENTS__H__
#define __PANELCOMPONENTS__H__
#include "Panel.h"
#include "ImGui/imgui.h"
#include "PanelComponents.h"
#include "../Source/ImGui/imgui_internal.h"


class PanelComponents :
	public Panel
{

public:
	PanelComponents();
	virtual ~PanelComponents();
	void Draw()override;



};
#endif
