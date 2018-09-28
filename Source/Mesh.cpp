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
	if (type == SPHERE_M)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(x, y, z);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
		glNormalPointer(GL_FLOAT, 0, &normals[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
		glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
		glPopMatrix();
	}
	//Enable Client
	glEnableClientState(GL_VERTEX_ARRAY);
	//Bind Vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//Bind Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
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

	glGenBuffers(1, (GLuint*) &(vertices_id));
	glGenBuffers(1, (GLuint*) &(indices_id));

	type = MESH_TYPE::CUBE_M;

	num_vertices = 8;
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
	vertices[2].y = 1.0f;
	vertices[2].z = 0.0f;
	//D
	vertices[3].x = 1.0f;
	vertices[3].y = 1.0f;
	vertices[3].z = 0.0f;
	//E
	vertices[4].x = 0.0f;
	vertices[4].y = 0.0f;
	vertices[4].z = 1.0f;
	//F
	vertices[5].x = 1.0f;
	vertices[5].y = 0.0f;
	vertices[5].z = 1.0f;
	//G
	vertices[6].x = 0.0f;
	vertices[6].y = 1.0f;
	vertices[6].z = 1.0f;
	//H
	vertices[7].x = 1.0f;
	vertices[7].y = 1.0f;
	vertices[7].z = 1.0f;

	//--------------
	glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices * 3, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	num_indices = 36;
	indices = new int[num_indices];
	//
	indices[0] = 0;	
	indices[1] = 1;	
	indices[2] = 2;	

	indices[3] = 2;	
	indices[4] = 1;	
	indices[5] = 3,
	
	indices[6] = 1;	
	indices[7] = 3;	
	indices[8] = 5;	
	
	indices[9] = 3;	
	indices[10] = 5;
	indices[11] = 7;

	indices[12] = 1; 
	indices[13] = 0; 
	indices[14] = 4;

	indices[15] = 1; 
	indices[16] = 4;
	indices[17] = 5;

	indices[18] = 7; 
	indices[19] = 5; 
	indices[20] = 4;

	indices[21] = 7; 
	indices[22] = 4; 
	indices[23] = 6;

	indices[24] = 3; 
	indices[25] = 7; 
	indices[26] = 2;

	indices[27] = 2; 
	indices[28] = 7; 
	indices[29] = 6;

	indices[30] = 6; 
	indices[31] = 4; 
	indices[32] = 0;

	indices[33] = 6; 
	indices[34] = 0;
	indices[35] = 2;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*num_indices, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CONSOLE_LOG("CUBE LOADED BUFFER_ID: %d", vertices_id);

}

void Mesh::DefinePlaneVertices()
{
	uint new_buffer;
	glGenBuffers(1, (GLuint*)&new_buffer);
	//Create Buffers
	vertices_id = new_buffer;

	uint new_buffer2;
	glGenBuffers(1, (GLuint*)&new_buffer2);
	indices_id = new_buffer2;
	//glGenBuffers(1, (GLuint*) &(vertices_id));
	//glGenBuffers(1, (GLuint*) &(indices_id));

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
	glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
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

void Mesh::DefineSphereVertices(float radius, uint rings, uint sectors)
{
	uint new_buffer;
	glGenBuffers(1, (GLuint*)&new_buffer);
	//Create Buffers
	vertices_id = new_buffer;

	uint new_buffer2;
	glGenBuffers(1, (GLuint*)&new_buffer2);
	indices_id = new_buffer2;

	type = MESH_TYPE::SPHERE_M;

	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);
	int r, s;
	int count = 0;

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			float const z = sin(-HALF_PI + PI * r * R);
			float const x = cos(2 * PI * s * S) * sin(PI * r * R);
			float const y = sin(2 * PI * s * S) * sin(PI * r * R);

			vertices[count].x = x * radius;
			vertices[count].y = y * radius;
			vertices[count].z = z * radius;

			count++;
		}
	}

	for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
		*indices++ = r * sectors + s;
		*indices++ = r * sectors + (s + 1);
		*indices++ = (r + 1) * sectors + (s + 1);
		*indices++ = (r + 1) * sectors + s;
	}

	glGenBuffers(1, (GLuint*)&vertices_id);
}
