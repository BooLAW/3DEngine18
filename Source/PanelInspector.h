#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"
#include "Material.h"
#include "ComponentMaterial.h"
#include "Mesh.h"


class GameObject;


class PanelInspector: public Panel
{
public:
	PanelInspector();
	virtual ~PanelInspector();
	void MeshComponentInfo(GameObject* active_GO);
	void Draw()override;
	
public:
	std::vector<uint> vertex;
	std::vector<uint> triangle;
	std::vector<int*> tex_coord;
	std::vector<const char*> mesh_name;

	std::vector<float3> positions;
	std::vector<float3> rotations;
	std::vector<float3> scales;


	std::vector<const char*> tex_name;
	std::vector<Material*> tex_data;

	uint counter = 0;
	const char* last_fbx = nullptr;

	

};

#endif