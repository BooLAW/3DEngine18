#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"

class GameObject;


class PanelInspector: public Panel
{
public:
	PanelInspector();
	virtual ~PanelInspector();
	void MeshComponentInfo(GameObject* active_GO);
	void Draw()override;
	
public:
	std::vector<uint> mesh_vertex;
	std::vector<int*> vertex;
	std::vector<const char*> mesh_name;
	int counter = 0;
};

#endif