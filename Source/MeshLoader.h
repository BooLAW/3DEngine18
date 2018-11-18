#ifndef __MESH_LOADER_H__
#define __MESH_LOADER_H__
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Module.h"
#include "OpenGL.h"


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
	bool LoadMesh(const std::string& file_name, bool create_go = true);	
	bool SaveSceneMeshesLW(const aiScene* scene, aiNode* node, const std::string &path);	
	bool SaveMeshBinary(const aiScene* scene, const aiNode* node,int num_mesh, const char* path);
	Mesh* LoadMeshBinary(const char* file_path);
	bool SaveMesh(const aiScene* scene, aiNode* node, const char* path);
	bool SaveMeshJson(const aiScene* scene, aiNode* node, Document* config);
	Mesh* LoadSceneMeshJson(std::string file_name,const aiNode* node);
	long fsize(FILE *fp);
	LineSegment CalculateTriangleNormal(float3 p1, float3 p2, float3 p3);
	bool InitMesh(std::string lw_path, GameObject* go);

	void Render();
	void CleanUp();

	//change to list when we have to load more than one mesh
	std::vector<Mesh*> scene_meshes;
	Material* mat;

private:
	bool InitMesh(const aiScene* scene, const aiNode* node, GameObject* parent,const char* path);
	void Absolute(LineSegment& line);

	
};


#endif // !__MESH__LOADER_H__
