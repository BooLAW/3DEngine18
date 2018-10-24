#include "MeshLoader.h"
#include "Mesh.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include <stdio.h>
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

bool MeshLoader::LoadMesh(const std::string &file_path)
{
	bool ret = false;
	const aiScene* new_scene = aiImportFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (new_scene != nullptr)
	{
		aiNode* root = new_scene->mRootNode;

		SaveSceneMeshesLW(new_scene, root,file_path);
		InitMesh(new_scene, root,App->scene_intro->scene_root,file_path.c_str());

		aiReleaseImport(new_scene);
	}
	else
		CONSOLE_LOG_ERROR("Error Loading the scene with name %s", file_path);


	return ret;
}

bool MeshLoader::SaveSceneMeshesLW(const aiScene* scene, aiNode* node, const std::string& path)
{

	char readBuf[100000];

	std::string file_name;
	file_name.append(App->GetFileName(path.c_str()));
	file_name.append(".lw");
	App->json->CreateNewJSON(file_name.c_str());
	json_file file1(file_name.c_str());
	FILE* fp = fopen(file_name.c_str(), "wb"); // non-Windows use "w"

	char writeBuffer[10000];

	Document testconfig_w;
	testconfig_w.SetObject();
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	SaveMesh(scene, node,&testconfig_w); //Starts Recursive


	Writer<FileWriteStream> writer(os);
	testconfig_w.Accept(writer);
	fclose(fp);

	


	return true;
}

bool MeshLoader::SaveMeshBinary(const aiScene * scene, const aiNode * node, int num_mesh)
{

	std::string final_file_name;
	final_file_name.append(node->mName.C_Str());
	final_file_name.append(".lw");

	FILE* wfile = fopen(final_file_name.c_str(), "wb");


	aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[num_mesh]];

	float3* vertices = (float3*)ai_mesh->mVertices;
	float* tex_points = new float[ai_mesh->mNumVertices * 3];
	// Vertices
	//--------------------------------------------------------------------- Save as .lw
	//We store the number of vertices inside an array
	uint header[3]; 
	header[0] = (uint)ai_mesh->mNumVertices * 3;
	header[1] = ai_mesh->mNumVertices;
	if (ai_mesh->HasTextureCoords(0))
	{
		header[2] = (uint)ai_mesh->mNumVertices;
	}
	uint bytes = sizeof(header);
	uint size = sizeof(header) + sizeof(float3)*ai_mesh->mNumVertices;
	char* sbuffer = new char[size];
	char* cursor = sbuffer;

	//And push it to the cursor
	bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	//Save Vertices
	bytes = sizeof(float3) * ai_mesh->mNumVertices;
	memcpy(cursor, vertices, bytes);
	cursor += bytes;

	//Save Tex_coord
	bytes = sizeof(float)*(uint)ai_mesh->mNumVertices * 3;
	memcpy(tex_points, ai_mesh->mTextureCoords[0], bytes);
	cursor += bytes;

	fwrite(sbuffer, sizeof(char), size, wfile);
	fclose(wfile);

	//------------------------------------------------------------------Load as .lw



	return false;
}

Mesh * MeshLoader::LoadMeshBinary(const aiScene * scene, const aiNode * node, int num_mesh)
{
	Mesh* ret = new Mesh();
	std::string final_file_name;
	final_file_name.append(node->mName.C_Str());
	final_file_name.append(".lw");
	aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[num_mesh]];

	//creates a file
	FILE* rfile = fopen(final_file_name.c_str(), "rb");

	//Init Header
	uint rheader[2];
	uint rbytes = sizeof(rheader);

	//Import Buffer
	uint rsize = sizeof(rheader) + sizeof(float3)*ai_mesh->mNumVertices;
	char* rbuffer = new char[rsize];
	fread(rbuffer, sizeof(char), rsize, rfile);
	char* rcursor = rbuffer;

	//Read Header
	memcpy(rheader, rcursor, rbytes);
	ret->num_vertices = rheader[0];
	rcursor += rbytes;

	//Read Vertices	
	rbytes = sizeof(float3) * ai_mesh->mNumVertices;
	float3* rvertices = new float3[ret->num_vertices];
	memcpy(rvertices, rcursor, rbytes);



	ret->vertices = rvertices;
	fclose(rfile);
	return ret;
}

bool MeshLoader::SaveMesh(const aiScene * scene, aiNode * node, Document* config)
{
	if (scene != nullptr && node->mNumMeshes > 0)
	{
		if (scene->HasMeshes())
		{
			Document::AllocatorType& allocator = config->GetAllocator();
			for (int i = 0; i < node->mNumMeshes; i++)
			{
				unsigned int total_size = 0;

				SaveMeshBinary(scene, node, i);

				aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];

				Value my_mesh(kObjectType);
				my_mesh.AddMember("num_vertices", ai_mesh->mNumVertices, allocator);
				my_mesh.AddMember("num_indices", (uint)ai_mesh->mNumFaces * 3, allocator);
				my_mesh.AddMember("num_normals", (uint)ai_mesh->mNumVertices * 3, allocator);
				my_mesh.AddMember("indices", (int)ai_mesh->mNumVertices * 3, allocator);

				//Vertices
				float3* points = new float3[ai_mesh->mNumVertices];
				memcpy(points, ai_mesh->mVertices, sizeof(float3) * ai_mesh->mNumVertices);
				total_size = sizeof(float3)*ai_mesh->mNumVertices;
				Value vertices_values(kArrayType);
				for (int i = 0; i < ai_mesh->mNumVertices; ++i)
				{
					int u = i + 1;
					int w = i + 2;
					points[i];
					points[u];
					points[w];

					vertices_values.PushBack((float)points[i].x, allocator);
					vertices_values.PushBack((float)points[i].y, allocator);
					vertices_values.PushBack((float)points[i].z, allocator);
				}

				my_mesh.AddMember("all_vertex", vertices_values, allocator);
				//Textures

				if (ai_mesh->HasTextureCoords(0))
				{
					my_mesh.AddMember("num_tex_coord", (uint)ai_mesh->mNumVertices, allocator);

					float* tex_points = new float[ai_mesh->mNumVertices * 3];
					memcpy(tex_points, ai_mesh->mTextureCoords[0], sizeof(float)*(uint)ai_mesh->mNumVertices * 3);
					Value texture_values(kArrayType);
					for (int i = 0; i<(uint)ai_mesh->mNumVertices; i++)
					{
						texture_values.PushBack((float)tex_points[i], allocator);
					}
					my_mesh.AddMember("texture_coords", texture_values, allocator);
				}

				Value n(node->mName.C_Str(), config->GetAllocator());
				config->AddMember(n, my_mesh, allocator);



			}
		}
	}
	
	for (int i = 0; i < node->mNumChildren; i++)
	{
		SaveMesh(scene, node->mChildren[i], config);
	}

	return false;
}

Mesh * MeshLoader::LoadSceneMeshLW(std::string file_name,const aiNode* node)
{
	//TODO JOSEP


	return nullptr;
}


LineSegment MeshLoader::CalculateTriangleNormal(float3 p1, float3 p2, float3 p3)
{
	LineSegment ret; //a = N, b = A

	float3 pv1;
	pv1.x = p1.x;
	pv1.y = p1.y;
	pv1.z = p1.z;

	float3 pv2;
	pv2.x = p2.x;
	pv2.y = p2.y;
	pv2.z = p2.z;

	float3 pv3;
	pv3.x = p3.x;
	pv3.y = p3.y;
	pv3.z = p3.z;

	float3 u = p2 - p1;
	float3 v = p3 - p1;

	float3 aux1 = pv2 - pv1;
	float3 retcros = aux1.Cross(pv3 - pv1);
	retcros = retcros.Normalized();

	ret.b = retcros;

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

			node->mTransformation.Decompose(aiScale, aiQuat, aiPos);

			float3 pos(aiPos.x, aiPos.y, aiPos.z);
			Quat rot(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
			float3 scale(aiScale.x, aiScale.y, aiScale.z);

			GO->transform->SetTransform(pos, rot, scale);


		}
		App->scene_intro->go_list.push_back(GO);
	}
	else
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
				LoadSceneMeshLW(App->scene_intro->fbx_name, node);

				

				Mesh* new_mesh = new Mesh();
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				Mesh* my_mesh2 = LoadMeshBinary(scene, node, i);
				//Vertices----------------------
				new_mesh->num_vertices = mesh->mNumVertices;
				new_mesh->vertices = new float3[new_mesh->num_vertices];				
				memcpy(new_mesh->vertices, mesh->mVertices, sizeof(float3) * new_mesh->num_vertices);
				

				glGenBuffers(1, (GLuint*)&new_mesh->vertices_id);
				glBindBuffer(GL_ARRAY_BUFFER, new_mesh->vertices_id);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * new_mesh->num_vertices, new_mesh->vertices, GL_STATIC_DRAW);

				CONSOLE_LOG_INFO("New mesh with:\n%d vertices", new_mesh->num_vertices);
				//reset buffer
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//Indices--------------------------
				if (mesh->HasFaces())
				{
					new_mesh->num_indices = mesh->mNumFaces * 3;
					new_mesh->num_normal = mesh->mNumVertices * 3;
					new_mesh->indices = new int[new_mesh->num_indices];
					

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
				if (node != nullptr) 
				{

					aiVector3D aiPos;
					aiQuaternion aiQuat;
					aiVector3D aiScale;

					node->mTransformation.Decompose(aiScale,aiQuat, aiPos);

					float3 pos(aiPos.x, aiPos.y, aiPos.z);
					Quat rot(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
					float3 scale(aiScale.x, aiScale.y, aiScale.z);

					new_child->transform->SetTransform(pos, rot, scale);


				}
				App->scene_intro->go_list.push_back(new_child);
				
				App->camera->AdaptCamera(bb,new_child->transform->transform.pos);
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


}
