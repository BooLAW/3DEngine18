#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "GameObject.h"
#include "OpenGL.h"
#include "Globals.h"
#include "DebugDraw.h"
#include "Material.h"

ComponentMesh::ComponentMesh()
{
	type = ComponentType::MESH;
}

ComponentMesh::ComponentMesh(GameObject * new_owner)
{
	owner = new_owner;
}

void ComponentMesh::AddMesh(Mesh * new_mesh)
{
	mesh = new_mesh;
}

bool ComponentMesh::Update()
{
	if (active == false)
		return false;
	//Enable Client
	glEnableClientState(GL_VERTEX_ARRAY);

	ComponentMaterial* my_material = (ComponentMaterial*)owner->GetComponent(ComponentType::MATERIAL);
	
	//Bind Vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertices_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//BindMaterial
	if (my_material != nullptr && my_material->data != nullptr)
	{
		if (my_material->data->textures_id > 0)
		{
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindTexture(GL_TEXTURE_2D, my_material->data->textures_id);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->tex_coords_id);
			glTexCoordPointer(3, GL_FLOAT, 0, NULL);
		}
		else
			glBindTexture(GL_TEXTURE_2D, 0);

	}
	//Bind Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_id);
	//Draw
	glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);
	//Unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Disable Client
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	if (mesh->show_bb)
	{
		DebugDraw(mesh, Red);
	}	
	return true;
}

void ComponentMesh::CleanUp()
{
}


ComponentMesh::~ComponentMesh()
{
}
