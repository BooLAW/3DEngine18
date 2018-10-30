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

void DebugDrawing(const Mesh* mesh, Color color, const float4x4 & transform)
{
	//glPushMatrix();
	//glMultMatrixf((GLfloat*)transform.Transposed().ptr());
	//glPopMatrix();
	//Bounding Box
	static float3 points[8];//is a box
	mesh->bounding_box.GetCornerPoints(points);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	glLineWidth(5.0f);
	BoxDD(points, color);
	glBegin(GL_LINES);
	
	//Normals
	/*glLineWidth(4.0f);
	for (uint i = 0; i < mesh->face_normal.size()-1; i++)
	{
		LineSegmentDraw(&mesh->face_normal[i].b, &mesh->face_normal[i].a,Blue);
	}*/
	glEnd();
	glLineWidth(1.0f);
	glColor3f(255, 255, 255);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BoxDD(const float3 * points, Color color, bool debug)
{
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
}


void LineSegmentDraw(const float3* origin,const float3* direction, Color color, bool debug)
{
	glColor3f(color.r, color.g, color.b);
	glVertex3fv((GLfloat*)&float3(origin->x, origin->y, origin->z));
	glVertex3fv((GLfloat*)&float3(direction->x, direction->y, direction->z));
}
