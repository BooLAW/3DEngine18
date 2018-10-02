#ifndef __MESH__H__
#define __MESH__H__

#include "Globals.h"
#include "Module.h"
#include "Assimp.h"

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

public:
	//Predefined Geometry
	void DefineCubeVertices(float3 new_position,float size);
	void DefinePlaneVertices(float3 new_position);
	void DefineArrowVertices(float3 new_position);
	void DefineSphereVertices(float radius, uint rings, uint sectors);

public:
	float3 color;
	MESH_TYPE type;
	//transform
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	//gl
	float3* vertices;
	int* indices;

	uint num_vertices;
	uint num_indices;

	uint vertices_id;
	uint indices_id;
};

#endif