#include "MeshLoader.h"
#include "Mesh.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include <stdio.h>
#include "glmath.h"
#include "Material.h"
#include "PanelInspector.h"
#include "Transform.h"
#include "ComponentTransform.h"
#include "mmgr/mmgr.h"

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
		CONSOLE_LOG_ERROR("Error Loading the scene with name %s", file_path);


	return ret;
}

bool MeshLoader::SaveMesh(Mesh* init_mesh)
{
	char readBuf[10000];

	App->json->CreateNewJSON("my_mesh.piformat");
	json_file file1("my_mesh.piformat");
	FILE* fp = fopen("my_mesh.piformat", "wb"); // non-Windows use "w"

	char writeBuffer[10000];
	Document testconfig_w;
	testconfig_w.SetObject();
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	Document::AllocatorType& allocator = testconfig_w.GetAllocator();
	Value app(kObjectType);
	app.AddMember("number of indices", init_mesh->num_indices, allocator);
	testconfig_w.AddMember("app", app, allocator);


	Writer<FileWriteStream> writer(os);
	testconfig_w.Accept(writer);
	fclose(fp);


	return false;
}


LineSegment MeshLoader::CalculateTriangleNormal(float3 p1, float3 p2, float3 p3)
{
	LineSegment ret; //a = N, b = A

	vec3 pv1;
	pv1.x = p1.x;
	pv1.y = p1.y;
	pv1.z = p1.z;

	vec3 pv2;
	pv2.x = p2.x;
	pv2.y = p2.y;
	pv2.z = p2.z;

	vec3 pv3;
	pv3.x = p3.x;
	pv3.y = p3.y;
	pv3.z = p3.z;

	float3 u = p2 - p1;
	float3 v = p3 - p1;

	vec3 retcros = cross(pv2 - pv1, pv3 - pv1);
	retcros = normalize(retcros);

	ret.b = (vec)&retcros;

	ret.a.x = (p1.x + p2.x + p3.x) / 3;
	ret.a.y = (p1.y + p2.y + p3.y) / 3;
	ret.a.z = (p1.z + p2.z + p3.z) / 3;
	ret.b = ret.b + ret.a;

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
			App->scene_intro->fbx_name = node_name;
		}
		else
		{
			GO->SetParent(App->scene_intro->scene_root);
		}
		GO->SetName(node_name.c_str());

		if (parent != nullptr)
			parent->SetChild(GO);

		//Transform
		if (node != nullptr) {

			aiVector3D aiPos;
			aiQuaternion aiQuat;
			aiVector3D aiScale;

			node->mTransformation.Decompose(aiPos, aiQuat, aiScale);

			float3 pos(aiPos.x, aiPos.y, aiPos.z);
			Quat rot(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
			float3 scale(aiScale.x, aiScale.y, aiScale.z);

			GO->transform->SetTransform(pos, rot, scale);


		}
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
				new_child->num_meshes = node->mNumMeshes;
				//MESH
				Mesh* new_mesh = new Mesh();
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				//Vertices----------------------
				new_mesh->num_vertices = mesh->mNumVertices;
				new_mesh->vertices = new float3[new_mesh->num_vertices];				
				memcpy(new_mesh->vertices, mesh->mVertices, sizeof(vec) * new_mesh->num_vertices);
				//memcpy(new_mesh->normal, mesh->mNormals, sizeof(float)*new_mesh->num_normal * 3);

				glGenBuffers(1, (GLuint*)&new_mesh->vertices_id);
				glBindBuffer(GL_ARRAY_BUFFER, new_mesh->vertices_id);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vec) * new_mesh->num_vertices, new_mesh->vertices, GL_STATIC_DRAW);

				CONSOLE_LOG_INFO("New mesh with:\n%d vertices", new_mesh->num_vertices);
				//reset buffer
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//Indices--------------------------
				if (mesh->HasFaces())
				{
					new_mesh->num_indices = mesh->mNumFaces * 3;
					new_mesh->num_normal = mesh->mNumVertices * 3;
					new_mesh->indices = new int[new_mesh->num_indices];
					new_mesh->normal = new float[new_mesh->num_normal];
					SaveMesh(new_mesh);

					for (int i = 0; i < mesh->mNumFaces; ++i)
					{
						if (mesh->mFaces[i].mNumIndices != 3) 
						{
							CONSOLE_LOG_WARNING("WARNING, face indices != 3");
						}
						else
						{
							memcpy(&new_mesh->indices[i * 3], mesh->mFaces[i].mIndices, sizeof(int) * 3);							
						}
					}
					for (int i = 0; i < mesh->mNumVertices; ++i)
					{
						int u = i + 1;
						int w = i + 2;
						LineSegment face_normal = CalculateTriangleNormal(new_mesh->vertices[i], new_mesh->vertices[u], new_mesh->vertices[w]);
						Absolute(face_normal);


						new_mesh->face_normal.push_back(face_normal);
					}
					glGenBuffers(1, (GLuint*)&new_mesh->indices_id);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_mesh->indices_id);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * new_mesh->num_indices, new_mesh->indices, GL_STATIC_DRAW);
					//reset buffer
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

					CONSOLE_LOG_INFO("%d indices", new_mesh->num_indices);

				}
				else
					CONSOLE_LOG_WARNING("Mesh has no Faces");


				//Tex Coords-------------------
				if (mesh->HasTextureCoords(0))
				{
					new_mesh->num_tex_coords = mesh->mNumVertices;
					new_mesh->tex_coords = new float[new_mesh->num_tex_coords * 3];
					memcpy(new_mesh->tex_coords, mesh->mTextureCoords[0], sizeof(float)*new_mesh->num_tex_coords * 3);


					glGenBuffers(1, (GLuint*)&new_mesh->tex_coords_id);
					glBindBuffer(GL_ARRAY_BUFFER, (GLuint)new_mesh->tex_coords_id);
					glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * new_mesh->num_tex_coords * 3, new_mesh->tex_coords, GL_STATIC_DRAW);

					//reset buffer
					CONSOLE_LOG_INFO("%d tex coords", new_mesh->num_tex_coords);

				}
				else
				{
					CONSOLE_LOG_WARNING("Mesh has no Texture Coords");
				}

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
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//Add child to parent
				parent->AddChild(new_child);
				//Transform
				if (node != nullptr) {

					aiVector3D aiPos;
					aiQuaternion aiQuat;
					aiVector3D aiScale;

					node->mTransformation.Decompose(aiPos, aiQuat, aiScale);

					float3 pos(aiPos.x, aiPos.y, aiPos.z);
					Quat rot(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
					float3 scale(aiScale.x, aiScale.y, aiScale.z);

					new_child->transform->SetTransform(pos, rot, scale);


				}
				App->scene_intro->go_list.push_back(new_child);
				
				App->camera->AdaptCamera(bb);
			}
			GO = parent;
		}
		else
		{
			CONSOLE_LOG_ERROR("Error loading FBX");
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
		//line.a.x = line.a.x * -1;
	}
	if (line.a.y < 0)
	{
		line.a.y = line.a.y * -1;
	}
	if (line.a.z < 0)
	{
		//line.a.z = line.a.z * -1;
	}
	/*
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
	*/

}
