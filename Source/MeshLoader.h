#ifndef __MESH_LOADER_H__
#define __MESH_LOADER_H__
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Module.h"
#include "OpenGL.h"
#include "glmath.h"

#define INVALID_MATERIAL 0xFFFFFFFF

#pragma comment (lib, "Assimp/libx86/assimp.lib")
class Mesh;
class GameObject;
class Material;
class MeshLoader
{
public:
	MeshLoader();
	virtual ~MeshLoader();
	bool LoadMesh(const std::string& file_name);
	LineSegment CalculateTriangleNormal(float3 p1, float3 p2, float3 p3);
	void Render();
	void CleanUp();
private:
	bool InitMesh(const aiScene* scene, const aiNode* node, GameObject* parent,const char* path);
	void Absolute(LineSegment& line);
public:
	//change to list when we have to load more than one mesh
	std::vector<Mesh*> scene_meshes;
	Material* mat;
};


#endif // !__MESH__LOADER_H__
