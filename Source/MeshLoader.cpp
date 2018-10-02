#include "MeshLoader.h"
#include "Mesh.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include <stdio.h>

MeshLoader::MeshLoader()
{
}


MeshLoader::~MeshLoader()
{
}

bool MeshLoader::LoadMesh(const std::string & file_path)
{
	bool ret = false;
	const aiScene* new_scene = aiImportFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (new_scene != nullptr && new_scene->HasMeshes())
	{
		//Iterate through meshes
		for (int i = 0; i < new_scene->mNumMeshes; i++)
		{
			const aiMesh* myMesh = new_scene->mMeshes[i];
			InitMesh(i, myMesh);
		}
		aiReleaseImport(new_scene);
	}
	else
		CONSOLE_LOG("Error Loading the scene with name %s", file_path);

	return ret;
}

void MeshLoader::Render()
{
}

void MeshLoader::CleanUp()
{
}

bool MeshLoader::InitMesh(uint i, const aiMesh * mesh)
{
	Mesh* new_mesh = new Mesh();
	//Vertices----------------------
	new_mesh->num_vertices = mesh->mNumVertices;
	new_mesh->vertices = new float3[new_mesh->num_vertices];
	memcpy(new_mesh->vertices, mesh->mVertices, sizeof(vec) * new_mesh->num_vertices);


	glGenBuffers(1, (GLuint*)&new_mesh->vertices_id);
	glBindBuffer(GL_ARRAY_BUFFER, new_mesh->vertices_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec) * new_mesh->num_vertices, new_mesh->vertices, GL_STATIC_DRAW);

	CONSOLE_LOG("%d vertices", new_mesh->num_vertices);
	//reset buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indices--------------------------
	if (mesh->HasFaces())
	{
		new_mesh->num_indices = mesh->mNumFaces * 3;
		new_mesh->indices = new int[new_mesh->num_indices];
		for (int i = 0; i < mesh->mNumFaces; ++i)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				CONSOLE_LOG("WARNING, face indices != 3");

			}
			else
			{
				memcpy(&new_mesh->indices[i * 3], mesh->mFaces[i].mIndices, sizeof(int) * 3);
			}
		}
	}
	glGenBuffers(1, (GLuint*)&new_mesh->indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_mesh->indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * new_mesh->num_indices, new_mesh->indices, GL_STATIC_DRAW);
	//reset buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CONSOLE_LOG("%d indices", new_mesh->indices);

	App->scene_intro->go_list.push_back(new_mesh);

	return true;
}
