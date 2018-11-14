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
	~ComponentMesh();

	void AddMesh(Mesh* new_mesh);
	bool Update();
	void DrawInspectorInfo();
	void CleanUp();
	void UpdateBoundingBox(float4x4 global_matrix);
	bool FirstPoint(LineSegment mouse_ray, float3& firstpoint, float& point_distance);
	Triangle CreateTriangleFromIndices(int i);
public:
	Mesh* mesh;

};
#endif