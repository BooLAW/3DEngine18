#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"
#include "Material.h"
#include "ComponentMaterial.h"
#include "Mesh.h"


class GameObject;


class PanelInspector : public Panel
{
public:
	PanelInspector();
	virtual ~PanelInspector();
	void MeshComponentInfo(GameObject* active_GO);
	void Draw()override;

public:

	uint counter = 0;
	GameObject* selected_go = nullptr;
	const char* last_fbx = nullptr;

	

};

#endif