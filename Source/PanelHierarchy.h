#ifndef __PANELHIERARCHY__H__
#define __PANELHIERARCHY__H__
#include "Panel.h"
#include "ImGui/imgui.h"
#include "PanelHierarchy.h"
#include "../Source/ImGui/imgui_internal.h"
#include "ModuleScene.h"
#include "Application.h"


class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();
	virtual ~PanelHierarchy();
	void Draw()override;

private:
	int id_new_sphere = 0;
	int id_new_cube = 0;
};
#endif

