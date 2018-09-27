#include "Mesh.h"
#include "OpenGL.h"


Mesh::Mesh()
{

}
Mesh::~Mesh()
{

}

void Mesh::Draw()
{
	//Enable Client
	glEnableClientState(GL_VERTEX_ARRAY);
	//Bind Vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);	//Bind Indices	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	//Draw
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	//Unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Disable Client
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Mesh::DefineCubeVertices()
{

}

void Mesh::DefinePlaneVertices()
{
	//Create Buffers
	glGenBuffers(1, (GLuint*) &(vertex_id));
	glGenBuffers(1, (GLuint*) &(indices_id));

	//Define Type

	type = MESH_TYPE::PLANE_M;
	//Define Vertices

	num_vertices = 4;
	vertices = new float3[num_vertices];

	//A
	vertices[0].x = 0.0f;
	vertices[0].y = 0.0f;
	vertices[0].z = 0.0f;
	//B
	vertices[1].x = 1.0f;
	vertices[1].y = 0.0f;
	vertices[1].z = 0.0f;
	//C
	vertices[2].x = 0.0f;
	vertices[2].y = 0.0f;
	vertices[2].z = 1.0f;
	//D
	vertices[3].x = 1.0f;
	vertices[3].y = 0.0f;
	vertices[3].z = 1.0f;

	//--------------
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices * 3, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Define Indices
	num_indices = 6;
	indices = new int[num_indices];
	//ABC
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	//CBD
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;

	//-----------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*num_indices, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


}
