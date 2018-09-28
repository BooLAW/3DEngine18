#ifndef __MESH__H__
#define __MESH__H__

#include "Globals.h"
#include "Module.h"
enum MESH_TYPE {
	CUBE_M = 0,
	PLANE_M,
	SPHERE_M,
	ARROW_M,
	CAPSULE_M,
	FRUSTUM_M
};
class Mesh
{
public:
	Mesh();
	virtual ~Mesh();


	void DefineCubeVertices();
	void DefinePlaneVertices();
	void DefineSphereVertices(float radius, uint rings, uint sectors);

	void Draw();



public:
	float3 color;
	MESH_TYPE type;

	float3* vertices;
	int* indices;

	uint num_vertices;
	uint num_indices;

	uint vertices_id;
	uint indices_id;
};

#endif