#include "MeshLoader.h"
#include "Mesh.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentRigidBody.h"
#include <stdio.h>
#include "Material.h"
#include "PanelInspector.h"
#include "Transform.h"
#include "ComponentTransform.h"
#include "mmgr/mmgr.h"
#include "Assimp/include/material.h"
#include <iostream>


MeshLoader::MeshLoader()
{
}


MeshLoader::~MeshLoader()
{
}

bool MeshLoader::LoadMesh(const std::string &file_path, bool create_go)
{
	bool ret = false;	
	std::string termination = App->loading_manager->GetTermination(file_path.c_str());
	std::size_t found_it = termination.find("lw");

	App->loading_manager->GetFileName(file_path.c_str());

	if (found_it == 0) // if it's an .lw file
	{
		//Create the Game Object
		GameObject* new_child;
		new_child = new GameObject();
		InitMesh(file_path, new_child);
		//Transform

		float3 pos(0, 0, 0);
		Quat rot(0, 0, 0, 0);
		float3 scale(1, 1, 1);
		new_child->comp_transform->SetTransform(pos, rot, scale);

		App->scene_intro->go_list.push_back(new_child);
		new_child->RecalculateBoundingBox(new_child);
		//App->camera->AdaptCamera(bb, new_child->comp_transform->transform.pos);
	}
	else
	{
		const aiScene* new_scene = aiImportFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		if (new_scene != nullptr)
		{
			aiNode* root = new_scene->mRootNode;
			
			
			SaveMesh(new_scene, root,file_path.c_str()); //Starts Recursive		
			if (create_go == true)
			{
				InitMesh(new_scene, root, App->scene_intro->scene_root, file_path.c_str());
			}
		
			aiReleaseImport(new_scene);
		}
		else
			CONSOLE_LOG_ERROR("Error Loading the scene with name %s", file_path);
	}
	return ret;
}

bool MeshLoader::InitMesh(std::string lw_path, GameObject* new_child)
{
	std::string subsctract_begining = lw_path;
	std::string substract_format;
	std::string substract_file_format;
	std::string final_file_name;
	std::size_t found_it = lw_path.find('_$');
	uint mesh_number = 0;
	if (found_it != UINT_MAX)
	{
		uint cut = lw_path.find_last_of('_$');
		subsctract_begining = lw_path.substr(cut+1, lw_path.length() - cut+1);
		substract_format = subsctract_begining;
		uint cut2 = subsctract_begining.find_last_of('.');
		substract_format = subsctract_begining.substr(0, cut2);
		mesh_number = std::stoi(substract_format.c_str());
	}
	substract_file_format = App->loading_manager->GetFileName(lw_path.c_str());

	uint cut3 = substract_file_format.find_last_of('.');
	final_file_name = substract_file_format.substr(0, cut3);

	Mesh* my_mesh = LoadMeshBinary(final_file_name.c_str());

	new_child->SetUID(App->loading_manager->GenerateNewUID());

	new_child->SetNumMeshes(mesh_number);
	//Vertices----------------------
	glGenBuffers(1, (GLuint*)&my_mesh->vertices_id);
	glBindBuffer(GL_ARRAY_BUFFER, my_mesh->vertices_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * my_mesh->num_vertices, my_mesh->vertices, GL_STATIC_DRAW);

	CONSOLE_LOG_INFO("New mesh with:\n%d vertices", my_mesh->num_vertices);

	//reset buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indices--------------------------
	if (my_mesh->num_vertices != 0) //Has Faces()
	{
		my_mesh->num_normal = my_mesh->num_vertices;
		for (int i = 0; i < my_mesh->num_vertices / 3; ++i)
		{
			int u = i + 1;
			int w = i + 2;
			LineSegment face_normal = CalculateTriangleNormal(my_mesh->vertices[i], my_mesh->vertices[u], my_mesh->vertices[w]);
			Absolute(face_normal);

			my_mesh->face_normal.push_back(face_normal);
		}
		glGenBuffers(1, (GLuint*)&my_mesh->indices_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_mesh->indices_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * my_mesh->num_indices, my_mesh->indices, GL_STATIC_DRAW);
		//reset buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		CONSOLE_LOG_INFO("%d indices", my_mesh->num_indices);

	}
	else
		CONSOLE_LOG_WARNING("Mesh has no Faces");

	//Tex Coords-------------------
	if (my_mesh->has_tex_coord == true) //Has Text Coords
	{
		my_mesh->num_tex_coords = my_mesh->num_vertices / 3;
		glGenBuffers(1, (GLuint*)&my_mesh->tex_coords_id);
		glBindBuffer(GL_ARRAY_BUFFER, (GLuint)my_mesh->tex_coords_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * my_mesh->num_tex_coords * 3, my_mesh->tex_coords, GL_STATIC_DRAW);

		//reset buffer
		CONSOLE_LOG_INFO("%d tex coords", my_mesh->num_tex_coords);
	}
	else
	{
		CONSOLE_LOG_WARNING("Mesh has no Texture Coords");
	}

	//Material
	if (my_mesh->material_index >= 0)
	{
		//Get png Name from the scene based on the mesh material id;
		aiString texture_name(my_mesh->material_path);
		
		//Loading the texture on the mesh from the assets folder										
		std::string tex_path("Assets/Textures/");
		if (texture_name.length > 0)
		{
			tex_path.append(texture_name.C_Str());
			std::string termination = App->loading_manager->GetTermination(texture_name.C_Str());
			ComponentMaterial* aux_cpm_mat = App->loading_manager->material_loader->LoadPNG(tex_path.c_str());
			new_child->PushComponent((Component*)aux_cpm_mat);
			
		}
		std::string importing_path = App->loading_manager->GetFolderNameFBX(my_mesh->file_path.c_str());
		App->loading_manager->ImportTextures(my_mesh->material_path, importing_path.c_str());
	}

	ComponentMesh*  new_comp_mesh = new ComponentMesh();
	new_comp_mesh->AddMesh(my_mesh);
	new_comp_mesh->SetOwner(new_child);
	new_comp_mesh->SetType(ComponentType::MESH);
	new_comp_mesh->Enable();
	new_comp_mesh->UpdateBoundingBox(new_comp_mesh->owner->comp_transform->trans_matrix_g);


	new_child->PushComponent((Component*)new_comp_mesh);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Add child to parent
	//parent->AddChild(new_child);

	return true;
}
bool MeshLoader::InitMesh(const aiScene* scene, const aiNode* node, GameObject* parent, const char* path)
{
	GameObject* GO;
	GO = new GameObject();

	if (node->mNumMeshes < 1)
	{
		std::string node_name(node->mName.C_Str());

		GO = new GameObject();

		if (node_name == "RootNode")//That's how assimp saves the rootnode
		{
			node_name = App->loading_manager->GetFileName(path);
			App->scene_intro->fbx_name = node_name;

			for (std::vector<GameObject*>::iterator it = App->scene_intro->go_list.begin(); it != App->scene_intro->go_list.end(); it++)
			{
				if ((*it)->IsRoot())
				{
					GO->SetParent((*it));
					GO->SetParentUID((*it)->GetUID());
				}
			}
	
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
		GO->SetUID(App->loading_manager->GenerateNewUID());
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
				
				new_child->SetUID(App->loading_manager->GenerateNewUID());

				new_child->SetName(node->mName.C_Str());
				new_child->SetNumMeshes(node->mNumMeshes);
				
				//MESH
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				
				//Get Component Mesh from the resources
				UINT32 mesh_uid = App->loading_manager->Find(node->mName.C_Str());
				Resource* res = App->loading_manager->resources[mesh_uid];
				ComponentMesh* mesh_res = (ComponentMesh*)res->GetComponent();
				Mesh* my_mesh = mesh_res->mesh;


				//Vertices----------------------
				glGenBuffers(1, (GLuint*)&my_mesh->vertices_id);
				glBindBuffer(GL_ARRAY_BUFFER, my_mesh->vertices_id);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * my_mesh->num_vertices, my_mesh->vertices, GL_STATIC_DRAW);

				CONSOLE_LOG_INFO("New mesh with:\n%d vertices", my_mesh->num_vertices);

				//reset buffer
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//Indices--------------------------
				if (my_mesh->num_vertices != 0) //Has Faces()
				{
					my_mesh->material_index = mesh->mMaterialIndex;
					my_mesh->num_normal = my_mesh->num_vertices;
					for (int i = 0; i < my_mesh->num_vertices/3; ++i)
					{
						int u = i + 1;
						int w = i + 2;
						LineSegment face_normal = CalculateTriangleNormal(my_mesh->vertices[i], my_mesh->vertices[u], my_mesh->vertices[w]);
						Absolute(face_normal);

						my_mesh->face_normal.push_back(face_normal);
					}
					glGenBuffers(1, (GLuint*)&my_mesh->indices_id);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_mesh->indices_id);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * my_mesh->num_indices, my_mesh->indices, GL_STATIC_DRAW);

					//reset buffer
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

					CONSOLE_LOG_INFO("%d indices", my_mesh->num_indices);

				}
				else
					CONSOLE_LOG_WARNING("Mesh has no Faces");

				//Tex Coords-------------------
				if (my_mesh->has_tex_coord == true) //Has Text Coords
				{										
					my_mesh->num_tex_coords = my_mesh->num_vertices/3;

					glGenBuffers(1, (GLuint*)&my_mesh->tex_coords_id);
					glBindBuffer(GL_ARRAY_BUFFER, (GLuint)my_mesh->tex_coords_id);
					glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * my_mesh->num_tex_coords * 3, my_mesh->tex_coords, GL_STATIC_DRAW);

					//reset buffer
					CONSOLE_LOG_INFO("%d tex coords", my_mesh->num_tex_coords);
				}
				else
				{
					CONSOLE_LOG_WARNING("Mesh has no Texture Coords");
				}				

				//Load Materials
				if (my_mesh->material_index >= 0)
				{
					//Get png Name from the scene based on the mesh material id;
					aiMaterial* mat = scene->mMaterials[my_mesh->material_index];				
					aiString texture_name;
					mat->GetTexture(aiTextureType_DIFFUSE, 0, &texture_name);
					aiColor3D my_color;
					mat->Get(AI_MATKEY_COLOR_DIFFUSE,my_color);					
					texture_name = App->loading_manager->GetFileName(texture_name.C_Str());
					
					//Create Materials Folder inside Library and FBX name folder inside Materials
					std::string input_path(App->scene_intro->fbx_name);
					std::string tex_folder_path;
					tex_folder_path.append("Library/Textures");
					CreateDirectory(tex_folder_path.c_str(), NULL); 

					//Creates BakerHouse Folder inside Materials folder
					tex_folder_path.append("/");
					tex_folder_path.append(App->loading_manager->EraseTerination(input_path.c_str()));
					CreateDirectory(tex_folder_path.c_str(), NULL);
					
					//Loading the texture on the mesh from the assets folder										
					std::string tex_path("Assets/Textures/");
					if (texture_name.length > 0)
					{	
						tex_path.append(texture_name.C_Str());
						std::string termination = App->loading_manager->GetTermination(texture_name.C_Str());
						ComponentMaterial* aux_cpm_mat = App->loading_manager->material_loader->LoadPNG(tex_path.c_str());
						new_child->PushComponent((Component*)aux_cpm_mat);
						my_mesh->color = { my_color[0],my_color[1],my_color[2] };
					}
					else
					{
						my_mesh->color = { my_color[0],my_color[1],my_color[2] };
					}
									
					//Storing the texture inside the Library
					std::string lib_tex_path("Library/Textures/");
					lib_tex_path.append(App->loading_manager->EraseTerination(input_path.c_str()));
					lib_tex_path.append("/");
					lib_tex_path.append(texture_name.C_Str());
					CopyFile(tex_path.c_str(), lib_tex_path.c_str(),NULL);				
				}

				ComponentMesh*  new_comp_mesh = mesh_res;
				new_comp_mesh->AddMesh(my_mesh);
				new_comp_mesh->SetOwner(new_child);
				new_comp_mesh->SetType(ComponentType::MESH);				
				new_comp_mesh->Enable();
				new_comp_mesh->UpdateBoundingBox(new_comp_mesh->owner->comp_transform->trans_matrix_g);

				new_child->PushComponent((Component*)new_comp_mesh);

				//ComponentRigidBody* new_comp_rigidbody = new ComponentRigidBody(new_child);

				//new_child->PushComponent((Component*)new_comp_rigidbody);
				
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//Add child to parent
				parent->AddChild(new_child);

				//Transform
				if (node != nullptr)
				{
					aiVector3D aiPos;
					aiQuaternion aiQuat;
					aiVector3D aiScale;

					node->mTransformation.Decompose(aiScale, aiQuat, aiPos);

					float3 pos(aiPos.x, aiPos.y, aiPos.z);
					Quat rot(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
					float3 scale(aiScale.x, aiScale.y, aiScale.z);

					new_child->comp_transform->SetTransform(pos, rot, scale);
				}
				App->scene_intro->go_list.push_back(new_child);
				new_child->RecalculateBoundingBox(new_child);
				App->camera->AdaptCamera(new_child->GetBB(), new_child->comp_transform->transform->pos);
			}	
		}
		else
		{
			CONSOLE_LOG_ERROR("Error loading FBX");
		}
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{

		InitMesh(scene, node->mChildren[i], GO, path);

	}
	GO->SetFirstUpdate(true);

	GO->comp_transform->UpdateTransformValues();

	return true;
}

bool MeshLoader::SaveMesh(const aiScene * scene, aiNode * node, const char* path)
{
	if (scene != nullptr && node->mNumMeshes > 0)
	{
		if (scene->HasMeshes())
		{
			for (int i = 0; i < node->mNumMeshes; i++)
			{
				SaveMeshBinary(scene, node, i,path);

			}
		}
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		SaveMesh(scene, node->mChildren[i], path);
	}

	return false;
}

bool MeshLoader::SaveMeshBinary(const aiScene * scene, const aiNode * node, int num_mesh, const char* path)
{
	std::string final_file_name;
	final_file_name.append(App->scene_intro->folder_path.c_str());
	std::string str("/");
	std::size_t found_it = final_file_name.find(str);
	if (found_it != std::string::npos)
	{
		final_file_name.append("/");
	}
	else
	{
		final_file_name.append("\\");
	}

	final_file_name.append(node->mName.C_Str());	
	if (num_mesh >= 1)
	{
		final_file_name.append("_$");
		final_file_name.append(std::to_string(num_mesh));
	}
	final_file_name.append(".lw");

	FILE* wfile = fopen(final_file_name.c_str(), "wb");
	if (wfile == NULL)
	{
		return false;
	}


	aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[num_mesh]];

	float3* vertices = (float3*)ai_mesh->mVertices;
	float* tex_points = new float[ai_mesh->mNumVertices * 3];

	//We store the number of vertices inside an array
	uint header[7] = { (uint)ai_mesh->mNumVertices * 3, ai_mesh->mNumVertices ,num_mesh};

	if (ai_mesh->HasTextureCoords(0))
	{
		header[3] = 1;
	}
	else
		header[3] = 0;

	if (ai_mesh->HasFaces())
	{
		header[4] = ai_mesh->mNumFaces * 3;
	}
	else
		header[4] = 0;

	aiString texture_name;
	float3 f_color;
	
	if (scene->HasMaterials())
	{
		aiMaterial* mat = scene->mMaterials[ai_mesh->mMaterialIndex];
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &texture_name);
		texture_name = App->loading_manager->GetFileName(texture_name.C_Str());
		aiColor3D my_color;
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, my_color);
		f_color = { my_color.r ,my_color.g,my_color.b };

		header[5] = ai_mesh->mMaterialIndex;		
		header[6] = sizeof(texture_name);
		
		App->loading_manager->ImportTextures(texture_name.C_Str(),path);
	}
	else
	{
		header[5] = -1;
		header[6] = 0;
	}		


	uint bytes = sizeof(header);
	uint size = sizeof(header) + sizeof(float3)*ai_mesh->mNumVertices + sizeof(float)*(uint)ai_mesh->mNumVertices * 3 + sizeof(int) * 3 * ai_mesh->mNumFaces + sizeof(texture_name) + sizeof(f_color);
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
		bytes = sizeof(int) *3* ai_mesh->mNumFaces;
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
		cursor += bytes;
		delete[] indices;
	}

	//Save Material
	if (scene->HasMaterials())
	{
		bytes = sizeof(texture_name);

		memcpy(cursor, texture_name.C_Str(), bytes);
		cursor += bytes;

		bytes = sizeof(f_color);
		memcpy(cursor, &f_color, bytes);
		
	}


	fwrite(sbuffer, sizeof(char), size, wfile);
	fclose(wfile);

	App->loading_manager->ImportFile(final_file_name.c_str());

	delete[] sbuffer;
	delete[] tex_points;

	return false;
}

Mesh * MeshLoader::LoadMeshBinary(const char* file_path)
{
	Mesh* ret = new Mesh();
	std::string final_file_name;
	std::string input_path(file_path);

	final_file_name.append(App->scene_intro->folder_path.c_str());
	final_file_name.append("/");
	final_file_name.append(file_path);
	//Scene does not load the format hence the if
	if (App->loading_manager->GetTermination(file_path) != "lw")
	{
		final_file_name.append(".lw");
	}

	ret->file_path.append(final_file_name.c_str());
	
	//creates a file
	FILE* rfile = fopen(final_file_name.c_str(), "rb");	
	
	//Import Buffer
	long rsize = fsize(rfile);
	char* rbuffer = new char[rsize];
	fread(rbuffer, sizeof(char), rsize, rfile);
	char* rcursor = rbuffer;

	//Init Header
	uint rheader[7];
	uint rbytes = sizeof(rheader);

	//Read Header
	memcpy(rheader, rcursor, rbytes);
	ret->num_vertices = rheader[0];

	rcursor += rbytes;

	//Read Vertices	
	rbytes = sizeof(float3) * ret->num_vertices/3;
	float3* rvertices = new float3[ret->num_vertices];
	memcpy(rvertices, rcursor, rbytes);

	//Store them in the mesh
	ret->vertices = new float3[ret->num_vertices];
	memcpy(ret->vertices, rvertices, rbytes);
	rcursor += rbytes;	

	if (rheader[3] == 1) //Has Texture Coords
	{
		ret->has_tex_coord = true;
		//Read tex_coord
		rbytes = sizeof(float)*(uint)ret->num_vertices;
		float* rtex_points = new float[ret->num_vertices];
		memcpy(rtex_points, rcursor, rbytes);
		

		//Store them in the mesh
		ret->tex_coords = new float[ret->num_vertices];		
		memcpy(ret->tex_coords, rtex_points, rbytes);
		rcursor += rbytes;

		delete[] rtex_points;
	}

	if (rheader[4] != 0) //Has Faces
	{
		ret->num_indices = rheader[4];

		//Read Indices
		rbytes = sizeof(int) * ret->num_indices;
		int* indices = new int[ret->num_indices];
		memcpy(indices, rcursor, rbytes);
		

		//Store them in the mesh
		ret->indices = new int[ret->num_indices];
		memcpy(ret->indices, indices, rbytes);
		delete[] indices;

		rcursor += rbytes;
	}

	if (rheader[5] >= 0)
	{
		//Read Indices and tex path
		rbytes = rheader[6];
		char* mat_name = new char[rheader[6]];
		memcpy(mat_name, rcursor, rbytes);

		//Store it in to the mesh
		ret->material_index = rheader[5];
		ret->material_path = mat_name;

		rcursor += rbytes;

		//Read bin folder
		rbytes = sizeof(float3);
		float3* aux_color = new float3[rbytes];
		memcpy(aux_color, rcursor, rbytes);

		//Store it in the mesh
		ret->color = { aux_color->x,aux_color->y,aux_color->z };
	}

	fclose(rfile);


	delete[] rbuffer;	
	delete[] rvertices;

	return ret;

}

long MeshLoader::fsize(FILE *fp)
{
	fseek(fp, 0, SEEK_END);
	long bytes = ftell(fp);
	rewind(fp);
	return bytes;
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

void MeshLoader::Absolute(LineSegment& line)
{
	if (line.a.x < 0)
	{
		//line.a.x = line.a.x * -1;
	}
	if (line.a.y < 0)
	{
		//line.a.y = line.a.y * -1;
	}
	if (line.a.z < 0)
	{
		//line.a.z = line.a.z * -1;
	}


}

bool MeshLoader::SaveSceneMeshesLW(const aiScene* scene, aiNode* node, const std::string& path)
{

	char readBuf[100000];

	std::string file_name;
	file_name.append(App->loading_manager->GetFileName(path.c_str()));
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