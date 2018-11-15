#pragma once
#include "Panel.h"
class PanelToolButtons :
	public Panel
{
public:
	PanelToolButtons();
	void Draw()override;
	virtual ~PanelToolButtons();
};

