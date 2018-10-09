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
	uint Vertex;
	std::string mesh_name;
	std::string mesh_data;
};

#endif