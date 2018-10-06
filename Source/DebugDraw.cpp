#include "DebugDraw.h"
#include "Globals.h"
#include "OpenGL.h"
#include "math.h"



void Init()
{
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	glColor4f(1.f, 1.f, 1.f, 1.f);
}

void CleanUp()
{
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void DebugDraw(const Mesh* mesh, Color color, const float4x4 & transform)
{
	//Bounding Box
	static float3 points[8];//is a box
	mesh->bounding_box.GetCornerPoints(points);

	glPushMatrix();
	glMultMatrixf((GLfloat*)transform.Transposed().ptr());
	BoxDD(points, color);
	glPopMatrix();

	//Normals


	for (uint i = 0; i < mesh->normal.size()-1; i++)
	{

		LineSegmentDraw(mesh->normal[i].a, mesh->normal[i].b*5, Red);
		

	}
	
}

void BoxDD(const float3 * points, Color color, bool debug)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	glDisable(GL_CULL_FACE);

	glColor3f(color.r, color.g, color.b);

	glBegin(GL_QUADS);

	glVertex3fv((GLfloat*)&points[1]); //glVertex3f(-sx, -sy, sz);
	glVertex3fv((GLfloat*)&points[5]); //glVertex3f( sx, -sy, sz);
	glVertex3fv((GLfloat*)&points[7]); //glVertex3f( sx,  sy, sz);
	glVertex3fv((GLfloat*)&points[3]); //glVertex3f(-sx,  sy, sz);

	glVertex3fv((GLfloat*)&points[4]); //glVertex3f( sx, -sy, -sz);
	glVertex3fv((GLfloat*)&points[0]); //glVertex3f(-sx, -sy, -sz);
	glVertex3fv((GLfloat*)&points[2]); //glVertex3f(-sx,  sy, -sz);
	glVertex3fv((GLfloat*)&points[6]); //glVertex3f( sx,  sy, -sz);

	glVertex3fv((GLfloat*)&points[5]); //glVertex3f(sx, -sy,  sz);
	glVertex3fv((GLfloat*)&points[4]); //glVertex3f(sx, -sy, -sz);
	glVertex3fv((GLfloat*)&points[6]); //glVertex3f(sx,  sy, -sz);
	glVertex3fv((GLfloat*)&points[7]); //glVertex3f(sx,  sy,  sz);

	glVertex3fv((GLfloat*)&points[0]); //glVertex3f(-sx, -sy, -sz);
	glVertex3fv((GLfloat*)&points[1]); //glVertex3f(-sx, -sy,  sz);
	glVertex3fv((GLfloat*)&points[3]); //glVertex3f(-sx,  sy,  sz);
	glVertex3fv((GLfloat*)&points[2]); //glVertex3f(-sx,  sy, -sz);

	glVertex3fv((GLfloat*)&points[3]); //glVertex3f(-sx, sy,  sz);
	glVertex3fv((GLfloat*)&points[7]); //glVertex3f( sx, sy,  sz);
	glVertex3fv((GLfloat*)&points[6]); //glVertex3f( sx, sy, -sz);
	glVertex3fv((GLfloat*)&points[2]); //glVertex3f(-sx, sy, -sz);

	glVertex3fv((GLfloat*)&points[0]); //glVertex3f(-sx, -sy, -sz);
	glVertex3fv((GLfloat*)&points[4]); //glVertex3f( sx, -sy, -sz);
	glVertex3fv((GLfloat*)&points[5]); //glVertex3f( sx, -sy,  sz);
	glVertex3fv((GLfloat*)&points[1]); //glVertex3f(-sx, -sy,  sz);

	glEnd();
	glLineWidth(1.0f);
	glColor3f(255, 255, 255);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void LineSegmentDraw(const vec origin,const vec direction, Color color, bool debug)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(6.0f);
	glDisable(GL_CULL_FACE);

	glColor3f(color.r, color.g, color.b);

	glBegin(GL_LINEAR);

	glVertex3fv((GLfloat*)&origin);
	glVertex3fv((GLfloat*)&direction);

	glEnd();
	glLineWidth(1.0f);
	glColor3f(255, 255, 255);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
