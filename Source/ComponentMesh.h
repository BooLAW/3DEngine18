#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"
#include "Mesh.h"

class GameObject;

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	ComponentMesh(GameObject* new_owner);
	virtual ~ComponentMesh();

	void AddMesh(Mesh* new_mesh);
	bool Update();
	void DrawInspectorInfo();
	void CleanUp();
	void UpdateBoundingBox();

public:
	Mesh* mesh;

};

#endif