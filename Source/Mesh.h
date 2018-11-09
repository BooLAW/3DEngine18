#ifndef __MESH__H__
#define __MESH__H__

#include "Globals.h"
#include "Module.h"
#include "Assimp.h"
#include "MathGeoLib/Geometry/Line.h"
#include "MathGeoLib/Geometry/LineSegment.h"

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
	void CalculateVertexNormal();

	bool BBNeedsUpdate();
	void BBToUpdate(bool active);
public:
	float3 color;
	MESH_TYPE type;
	//transform
	float3 position = float3::zero;
	std::string file_path;

	//GL--------------------

	//Vertices
	float3* vertices;
	uint num_vertices;
	uint vertices_id;

	//Normal
	std::vector<LineSegment> face_normal;
	std::vector<LineSegment> normals;
	float* normal;
	uint num_normal = 0;

	//Indices
	int* indices;
	uint num_indices;
	uint indices_id;

	//Tex Coords
	bool has_tex_coord = false;
	float* tex_coords;
	uint num_tex_coords;
	uint tex_coords_id;
	uint material_index;

	//AABB
	AABB bounding_box;
	bool update_bb;

	uint id;

	//Faces
	uint num_faces;

	bool show_bb;
	bool show_normal;

};

#endif