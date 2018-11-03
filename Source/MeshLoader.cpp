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
		SaveMesh(new_scene,root); //Starts Recursive
		//SaveSceneMeshesLW(new_scene, root,file_path);
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

	FILE* fp = fopen(file_name.c_str(), "wb"); // non-Windows use "w"

	char writeBuffer[10000];

	Document testconfig_w;
	testconfig_w.SetObject();
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	

	Writer<FileWriteStream> writer(os);
	testconfig_w.Accept(writer);
	fclose(fp);

	return true;
}

bool MeshLoader::SaveMeshBinary(const aiScene * scene, const aiNode * node, int num_mesh)
{
	std::string final_file_name;
	final_file_name.append("Assets/Models/");
	final_file_name.append(node->mName.C_Str());
	if (node->mNumMeshes > 1)
	{
		final_file_name.append(std::to_string(num_mesh));
	}
	final_file_name.append(".lw");

	FILE* wfile = fopen(final_file_name.c_str(), "wb");

	aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[num_mesh]];

	float3* vertices = (float3*)ai_mesh->mVertices;
	float* tex_points = new float[ai_mesh->mNumVertices * 3];
	//We store the number of vertices inside an array
	uint header[4] = { (uint)ai_mesh->mNumVertices * 3,ai_mesh->mNumVertices };

	if (ai_mesh->HasTextureCoords(0))
	{
		header[2] = (uint)ai_mesh->mNumVertices;
	}
	if (ai_mesh->HasFaces())
	{
		header[3] = ai_mesh->mNumFaces * 3;
	}
	uint bytes = sizeof(header);
	uint size = sizeof(header) + sizeof(float3)*ai_mesh->mNumVertices + sizeof(float)*(uint)ai_mesh->mNumVertices * 3 + sizeof(int) * 3 * ai_mesh->mNumFaces;
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

	if (ai_mesh->HasTextureCoords(0))
	{
		//Save Tex_coord
		bytes = sizeof(float)*(uint)ai_mesh->mNumVertices * 3;
		memcpy(cursor, ai_mesh->mTextureCoords[0], bytes);
		cursor += bytes;
	}


	//Save Indices
	if (ai_mesh->HasFaces())
	{		
		bytes = sizeof(int) * 3 * ai_mesh->mNumFaces;
		int* indices = new int[ai_mesh->mNumFaces * 3 ];
		for (int i = 0; i < ai_mesh->mNumFaces; ++i)
		{
			if (ai_mesh->mFaces[i].mNumIndices != 3)
			{
				CONSOLE_LOG_WARNING("WARNING, face indices != 3");
			}
			else
			{

				memcpy(&indices[i * 3], ai_mesh->mFaces[i].mIndices, sizeof(int) * 3);
			}
		}
		memcpy(cursor, indices, bytes);
	}

	/*
	my_mesh2->num_indices = mesh->mNumFaces * 3;
	my_mesh2->num_normal = mesh->mNumVertices * 3;
	my_mesh2->indices = new int[my_mesh2->num_indices];


	for (int i = 0; i < mesh->mNumFaces; ++i)
	{
		if (mesh->mFaces[i].mNumIndices != 3)
		{
			CONSOLE_LOG_WARNING("WARNING, face indices != 3");
		}
		else
		{
			memcpy(&my_mesh2->indices[i * 3], mesh->mFaces[i].mIndices, sizeof(int) * 3);
		}
	}
	*/

	fwrite(sbuffer, sizeof(char), size, wfile);
	fclose(wfile);

	return false;
}

Mesh * MeshLoader::LoadMeshBinary(const aiScene * scene, const aiNode * node, int num_mesh)
{
	Mesh* ret = new Mesh();
	std::string final_file_name;
	final_file_name.append("Assets/Models/");
	final_file_name.append(node->mName.C_Str());
	if (node->mNumMeshes > 1)
	{
		final_file_name.append(std::to_string(num_mesh));
	}
	final_file_name.append(".lw");
	aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[num_mesh]];

	//creates a file
	FILE* rfile = fopen(final_file_name.c_str(), "rb");

	//Init Header
	uint rheader[4];
	uint rbytes = sizeof(rheader);

	//Import Buffer
	uint rsize = sizeof(rheader) + sizeof(float3)*ai_mesh->mNumVertices + sizeof(float)*(uint)ai_mesh->mNumVertices * 3 + sizeof(int) * 3 * ai_mesh->mNumFaces;
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

	//Store them in the mesh
	ret->vertices = new float3[ret->num_vertices];
	memcpy(ret->vertices, rvertices, rbytes);
	rcursor += rbytes;

	if (ai_mesh->HasTextureCoords(0))
	{
		//Read tex_coord
		rbytes = sizeof(float)*(uint)ai_mesh->mNumVertices * 3;
		float* rtex_points = new float[ai_mesh->mNumVertices * 3];
		memcpy(rtex_points, rcursor, rbytes);
		

		//Store them in the mesh
		ret->tex_coords = new float[ai_mesh->mNumVertices * 3];
		memcpy(ret->tex_coords, rtex_points, rbytes);
		rcursor += rbytes;
	}

	if (ai_mesh->HasFaces())
	{
		ret->num_indices = rheader[3];
		//Read Indices
		rbytes = sizeof(int) * 3 * ai_mesh->mNumFaces;
		int* indices = new int[ai_mesh->mNumFaces * 3];
		memcpy(indices, rcursor, rbytes);
		

		//Store them in the mesh
		ret->indices = new int[ai_mesh->mNumFaces * 3];
		memcpy(ret->indices, indices, rbytes);
		rcursor += rbytes;
	}

	

	fclose(rfile);
	return ret;
}

bool MeshLoader::SaveMesh(const aiScene * scene, aiNode * node)
{
	if (scene != nullptr && node->mNumMeshes > 0)
	{
		if (scene->HasMeshes())
		{
			//SaveMeshJson(scene, node, config);

			for (int i = 0; i < node->mNumMeshes; i++)
			{
				SaveMeshBinary(scene, node, i);
			
			}
		}
	}
	
	for (int i = 0; i < node->mNumChildren; i++)
	{
		SaveMesh(scene, node->mChildren[i]);
	}

	return false;
}

bool MeshLoader::SaveMeshJson(const aiScene * scene, aiNode * node, Document * config)
{
	Document::AllocatorType& allocator = config->GetAllocator();
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		unsigned int total_size = 0;

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

	return false;
}

Mesh * MeshLoader::LoadSceneMeshJson(std::string file_name,const aiNode* node)
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
			node_name = App->GetFileName(path);
			App->scene_intro->fbx_name = node_name;
			GO->SetParent(App->scene_intro->scene_root);
		}

		else if (parent != nullptr)
		{
			parent->AddChild(GO);

		}
		GO->SetName(node_name.c_str());

		//Transform
		if (node != nullptr) {

			aiVector3D aiPos;
			aiQuaternion aiQuat;
			aiVector3D aiScale;

			node->mTransformation.Decompose(aiScale, aiQuat, aiPos);

			float3 pos(aiPos.x, aiPos.y, aiPos.z);
			Quat rot(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
			float3 scale(aiScale.x, aiScale.y, aiScale.z);

			GO->comp_transform->SetTransform(pos, rot, scale);			
		}

		//Create Random UID for Root
		unsigned int max_int = UINT_MAX;
		UINT32 random_int = pcg32_boundedrand_r(&App->imgui->rng, max_int) + 1000000000;
		GO->uid = random_int;

		App->scene_intro->go_list.push_back(GO);
	}
	else
	{
		if (scene != nullptr && scene->HasMeshes())
		{
			for (int i = 0; i < node->mNumMeshes; i++)
			{
				//Create the Game Object
				GameObject* new_child = new GameObject();

				//Create Random UID for new_child
				unsigned int max_int = UINT_MAX;
				UINT32 random_int = pcg32_boundedrand_r(&App->imgui->rng, max_int) + 1000000000;
				new_child->uid = random_int;

				new_child->SetName(node->mName.C_Str());
				new_child->num_meshes = node->mNumMeshes;

				//MESH
				LoadSceneMeshJson(App->scene_intro->fbx_name, node);
			
				//Mesh* my_mesh2 = new Mesh();
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				Mesh* my_mesh2 = LoadMeshBinary(scene, node, i);
				//Vertices----------------------
				//my_mesh2->num_vertices = mesh->mNumVertices;
				//my_mesh2->vertices = new float3[my_mesh2->num_vertices];				
				//memcpy(my_mesh2->vertices, mesh->mVertices, sizeof(float3) * my_mesh2->num_vertices);
				

				glGenBuffers(1, (GLuint*)&my_mesh2->vertices_id);
				glBindBuffer(GL_ARRAY_BUFFER, my_mesh2->vertices_id);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * my_mesh2->num_vertices, my_mesh2->vertices, GL_STATIC_DRAW);

				CONSOLE_LOG_INFO("New mesh with:\n%d vertices", my_mesh2->num_vertices);
				//reset buffer
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//Indices--------------------------
				if (mesh->HasFaces())
				{

					//my_mesh2->num_indices = mesh->mNumFaces * 3;
					my_mesh2->num_normal = mesh->mNumVertices * 3;

					//my_mesh2->indices = new int[my_mesh2->num_indices];
				


					for (int i = 0; i < mesh->mNumFaces; ++i)
					{
						if (mesh->mFaces[i].mNumIndices != 3) 
						{
							CONSOLE_LOG_WARNING("WARNING, face indices != 3");
						}
						else
						{
							//memcpy(&my_mesh2->indices[i * 3], mesh->mFaces[i].mIndices, sizeof(int) * 3);							
						}
					}
					for (int i = 0; i < mesh->mNumVertices; ++i)
					{
						int u = i + 1;
						int w = i + 2;
						LineSegment face_normal = CalculateTriangleNormal(my_mesh2->vertices[i], my_mesh2->vertices[u], my_mesh2->vertices[w]);
						Absolute(face_normal);


						my_mesh2->face_normal.push_back(face_normal);
					}
					glGenBuffers(1, (GLuint*)&my_mesh2->indices_id);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_mesh2->indices_id);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * my_mesh2->num_indices, my_mesh2->indices, GL_STATIC_DRAW);
					//reset buffer
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

					CONSOLE_LOG_INFO("%d indices", my_mesh2->num_indices);

				}
				else
					CONSOLE_LOG_WARNING("Mesh has no Faces");


				//Tex Coords-------------------
				if (mesh->HasTextureCoords(0))
				{
					my_mesh2->num_tex_coords = mesh->mNumVertices;
					//my_mesh2->tex_coords = new float[my_mesh2->num_tex_coords * 3];
					//memcpy(my_mesh2->tex_coords, mesh->mTextureCoords[0], sizeof(float)*my_mesh2->num_tex_coords * 3);


					glGenBuffers(1, (GLuint*)&my_mesh2->tex_coords_id);
					glBindBuffer(GL_ARRAY_BUFFER, (GLuint)my_mesh2->tex_coords_id);
					glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * my_mesh2->num_tex_coords * 3, my_mesh2->tex_coords, GL_STATIC_DRAW);

					//reset buffer
					CONSOLE_LOG_INFO("%d tex coords", my_mesh2->num_tex_coords);
				}
				else
				{
					CONSOLE_LOG_WARNING("Mesh has no Texture Coords");
				}

				//Set the Bounding Box for the DEBUG DRAW
				AABB bb;
				bb.SetNegativeInfinity();
				bb.Enclose((float3*)mesh->mVertices, mesh->mNumVertices);
				my_mesh2->bounding_box = bb;
				my_mesh2->show_bb = false;
			


				ComponentMesh*  new_comp_mesh = new ComponentMesh();
				new_comp_mesh->AddMesh(my_mesh2);
				new_comp_mesh->SetOwner(new_child);
				new_comp_mesh->SetType(ComponentType::MESH);
				new_comp_mesh->Enable();

				new_child->PushComponent((Component*)new_comp_mesh);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//Add child to parent
				parent->AddChild(new_child);
				new_child->parent_uid = parent->uid;

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

					new_child->comp_transform->SetTransform(pos, rot, scale);


				}
				App->scene_intro->go_list.push_back(new_child);
				new_child->RecalculateBoundingBox(new_child);
				App->camera->AdaptCamera(bb,new_child->comp_transform->transform->pos);
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
	GO->comp_transform->UpdateTransformValues();

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
