#include "MeshLoader.h"
#include "Mesh.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
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

	if (new_scene != nullptr)
	{
		aiNode* root = new_scene->mRootNode;

		InitMesh(new_scene, root,App->scene_intro->scene_root,file_path.c_str());

		aiReleaseImport(new_scene);
	}
	else
		CONSOLE_LOG("Error Loading the scene with name %s", file_path);

	return ret;
}

LineSegment MeshLoader::CalculateTriangleNormal(float3 p1, float3 p2, float3 p3)
{
	LineSegment ret; //a = N, b = A
	float3 v = p2 - p1;
	float3 w = p3 - p1;
	ret.a.x = (v.y * w.z) - (v.z * w.y);
	ret.a.y = (v.z * w.x) - (v.x * w.z);
	ret.a.z = (v.x * w.y) - (v.y * w.x);

	ret.b.x = ret.a.x / (ret.a.x + ret.a.y + ret.a.z);
	ret.b.y = ret.a.y / (ret.a.x + ret.a.y + ret.a.z);
	ret.b.z = ret.a.z / (ret.a.x + ret.a.y + ret.a.z);

	return ret;
}




void MeshLoader::Render()
{
}

void MeshLoader::CleanUp()
{
	aiDetachAllLogStreams();
}

bool MeshLoader::InitMesh(const aiScene* scene,const aiNode* node, GameObject* parent,const char* path)
{
	GameObject* GO = new GameObject();
	if (node->mNumMeshes < 1)
	{
		std::string node_name(node->mName.C_Str());

		GO = new GameObject();

		node_name = node->mName.C_Str();

		if (node_name == "RootNode")//That's how assimp saves the rootnode
		{
			GO->root_go = true;
			node_name = App->GetFileName(path);
		}
		else
		{
			GO->SetParent(App->scene_intro->scene_root);
		}
		GO->SetName(node_name.c_str());


		if (parent != nullptr)
			parent->SetChild(GO);


		App->scene_intro->go_list.push_back(GO);
	}else
	{
		if (scene != nullptr && scene->HasMeshes())
		{
			for (int i = 0; i < node->mNumMeshes; i++)
			{
				//Put the name
				GameObject* new_child = new GameObject();
				new_child->SetName(node->mName.C_Str());

				//MESH
				Mesh* new_mesh = new Mesh();
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				//Vertices----------------------
				new_mesh->num_vertices = mesh->mNumVertices;
				new_mesh->vertices = new float3[new_mesh->num_vertices];
				memcpy(new_mesh->vertices, mesh->mVertices, sizeof(vec) * new_mesh->num_vertices);


				glGenBuffers(1, (GLuint*)&new_mesh->vertices_id);
				glBindBuffer(GL_ARRAY_BUFFER, new_mesh->vertices_id);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vec) * new_mesh->num_vertices, new_mesh->vertices, GL_STATIC_DRAW);

				CONSOLE_LOG("New mesh with %d vertices", new_mesh->num_vertices);
				//reset buffer
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//Indices--------------------------
				if (mesh->HasFaces())
				{
					new_mesh->num_indices = mesh->mNumFaces * 3;
					new_mesh->indices = new int[new_mesh->num_indices];
					for (int i = 0; i < mesh->mNumFaces; ++i)
					{
						if (mesh->mFaces[i].mNumIndices != 3) //Detects if there is something that is not a triangle
						{
							CONSOLE_LOG("WARNING, face indices != 3");
						}
						else
						{
							memcpy(&new_mesh->indices[i * 3], mesh->mFaces[i].mIndices, sizeof(int) * 3);

							////if (i % 3 == 0)
							//{
							//	LineSegment normal = CalculateTriangleNormal(new_mesh->vertices[i], new_mesh->vertices[i + 1], new_mesh->vertices[i + 2]);
							//	Absolute(normal);
							//	new_mesh->normal.push_back(normal);
							//}
						}
					}
					glGenBuffers(1, (GLuint*)&new_mesh->indices_id);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_mesh->indices_id);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * new_mesh->num_indices, new_mesh->indices, GL_STATIC_DRAW);
					//reset buffer
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

					CONSOLE_LOG("%d indices", new_mesh->num_indices);
				}
				else
					CONSOLE_LOG("Mesh has no Faces");

				//TODO fix tex coords

				//Tex Coords-------------------
				//if (mesh->HasTextureCoords(0))
				//{
				//	new_mesh->num_tex_coords = mesh->mNumVertices;
				//	new_mesh->tex_coords = new float[new_mesh->num_tex_coords * 3];
				//	memcpy(&new_mesh->tex_coords, mesh->mTextureCoords[0], sizeof(float)*new_mesh->num_tex_coords * 3);


				//	glGenBuffers(1, (GLuint*)&new_mesh->tex_coords_id);
				//	glBindBuffer(GL_ARRAY_BUFFER, new_mesh->tex_coords_id);
				//	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * new_mesh->num_tex_coords * 3, new_mesh->tex_coords, GL_STATIC_DRAW);

				//	//reset buffer
				//	glBindBuffer(GL_ARRAY_BUFFER, 0);
				//	CONSOLE_LOG("%d tex coords", new_mesh->num_tex_coords);

				//}
				//else
				//{
				//	CONSOLE_LOG("Mesh has no Texture Coords");
				//}

				//Set the Bounding Box for the DEBUG DRAW
				AABB bb;
				bb.SetNegativeInfinity();
				bb.Enclose((float3*)mesh->mVertices, mesh->mNumVertices);
				new_mesh->bounding_box = bb;
				new_mesh->show_bb = false;


				ComponentMesh*  new_comp_mesh = new ComponentMesh();
				new_comp_mesh->AddMesh(new_mesh);
				new_comp_mesh->SetOwner(new_child);
				new_comp_mesh->SetType(ComponentType::MESH);
				new_comp_mesh->Enable();

				new_child->PushComponent((Component*)new_comp_mesh);
				//Materials
				if (scene != nullptr && scene->HasMaterials())
				{
					aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
					aiString aipath;

					ComponentMaterial* aux_mat = new ComponentMaterial();
					material->GetTexture(aiTextureType_DIFFUSE, 0, &aipath);
					std::string path = aipath.C_Str();
					uint name_pos = path.find_last_of("\\");

					aux_mat->type = ComponentType::MATERIAL;
					aux_mat->Enable();
					//set component owner
					aux_mat->SetOwner(new_child);
					//Add component to child
					new_child->PushComponent(aux_mat);
				}
				//Add child to parent
				parent->AddChild(new_child);
				//Transform
				if (node != nullptr) {
					//TODO
				}
				App->scene_intro->go_list.push_back(new_child);
			}
			GO = parent;
		}
		else
		{
			CONSOLE_LOG("Error loading FBX");
		}
		
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		InitMesh(scene, node->mChildren[i], GO,path);
	}
		
	

	return true;
}

void MeshLoader::Absolute(LineSegment& line)
{
	if (line.a.x < 0)
	{
		line.a.x = line.a.x * -1;
	}
	if (line.a.y < 0)
	{
		line.a.y = line.a.y * -1;
	}
	if (line.a.z < 0)
	{
		line.a.z = line.a.z * -1;
	}

	if (line.b.x < 0)
	{
		line.b.x = line.b.x * -1;
	}
	if (line.b.y < 0)
	{
		line.b.y = line.b.y * -1;
	}
	if (line.b.z < 0)
	{
		line.b.z = line.b.z * -1;
	}
}
