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


	void Draw();

	void DefineCubeVertices();
	void DefinePlaneVertices();

public:
	float3 color;
	MESH_TYPE type;

	float3* vertices;
	int* indices;

	uint num_vertices;
	uint num_indices;

	uint vertex_id;
	uint indices_id;
};

#endif