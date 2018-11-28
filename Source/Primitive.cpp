#include "Primitive.h"
#include "OpenGL.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Module.h"
#include <gl/GL.h>
#include <gl/GLU.h>


Primitive::Primitive() :transform(float4x4::identity), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{
}

void Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.ptr());

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	if (axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);
	glEnable(GL_TEXTURE_2D);
	
	if (App->renderer3D->attributes.lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);	
	
	InnerRender();

	/*
	//Draw Line
	glLineWidth(5.0f);
	glBegin(GL_TRIANGLES);
	glVertex3f(0.f, 0.f, 20.f);
	glVertex3f(0.f, 5.f, 20.f);
	glVertex3f(5.f, 0.f, 20.f);

	glVertex3f(0.f, 5.f, 20.f);
	glVertex3f(5.f, 5.f, 20.f);
	glVertex3f(5.f, 0.f, 20.f);

	glVertex3f(5.f, 5.f, 20.f);
	glVertex3f(5.f, 0.f, 20.f);
	glVertex3f(5.f, 0.f, 25.f);

	glVertex3f(0.f, 0.f, 20.f);
	glVertex3f(5.f, 0.f, 25.f);
	glVertex3f(0.f, 0.f, 25.f);
	glEnd();

	glLineWidth(1.0f);*/


	glPopMatrix();
}

void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

void Primitive::SetPos(float x, float y, float z)
{
	transform.Translate(x, y, z);
}

void Primitive::SetRotation(float angle, const float3 & u)
{
	transform.RotateAxisAngle(u,angle);
}

void Primitive::Scale(float x, float y, float z)
{
	transform.Scale(x, y, z);
}

PrimitiveTypes Primitive::GetType() const
{
	return type;
}


//CUBE=====================================================

// PLANE ==================================================
PPlane::PPlane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

PPlane::PPlane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void PPlane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}

PCube::PCube():Primitive()
{
	type = PrimitiveTypes::Primitive_Cube;

}

PCube::PCube(float x, float y, float z):Primitive(), dimensions(x,y,z)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void PCube::InnerRender() const
{
	//draw direct mode cube

	float sx = dimensions.x * 0.5f;
	float sy = dimensions.y * 0.5f;
	float sz = dimensions.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(-sx, sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, sy, -sz);
	glVertex3f(sx, sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(sx, sy, sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(-sx, sy, sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(-sx, -sy, sz);

	glEnd();
}

PSphere::PSphere() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

PSphere::PSphere(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void PSphere::InnerRender() const
{
	//glutSolidSphere(radius, 25, 25);
	Sphere(pos, radius);
}