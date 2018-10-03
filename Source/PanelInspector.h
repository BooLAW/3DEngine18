#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"
class PanelInspector: public Panel
{
public:
	PanelInspector(const char* name);
	virtual ~PanelInspector();
};

#endif