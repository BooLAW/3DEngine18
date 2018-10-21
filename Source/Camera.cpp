#include "Camera.h"
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include "ModuleCamera3D.h"
#include "Application.h"
#include "TextureMSAA.h"

Camera::Camera()
{
	//frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);

	CalculateViewMatrix();
	frustum.SetPos(float3(0, 1, -1));
	frustum.SetFront(float3(0, 0, 1));
	frustum.SetUp(float3(0, 1, 0));

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(0.0f, 0.0f, 5.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);
	frustum.horizontalFov = DegToRad(120);
	frustum.verticalFov = DegToRad(60);
	frustum.nearPlaneDistance = 0.5;//needs to be higher than 0.4
	frustum.farPlaneDistance = 800;

	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.SetWorldMatrix(float3x4::identity);

	//frustum.Translate(float3(0,0,0));
	//frustum.pos = Position;
	frustum.pos = float3(0, 0, 0);

	frustum.Translate(Position);

	CreateNewFrustum();
	
}

Camera::~Camera()
{
}

void Camera::SetPosition(const float3 & new_pos)
{
	frustum.SetPos(new_pos);
	Position = frustum.pos;
}

void Camera::SetReference(const float3 & new_pos)
{
	Reference = new_pos;
}

void Camera::SetFront(const float3 & front)
{
	frustum.SetFront(front);
}

void Camera::SetUp(const float3 & up)
{
	frustum.SetUp(up);
}

void Camera::SetFOV(const float & new_fov)
{
	if (new_fov <= 0)
	{
		CONSOLE_LOG_WARNING("Can't set a fov of negative value : %d", new_fov);
	}
	else
	{
		vertical_fov = new_fov * DEGTORAD;
		frustum.SetVerticalFovAndAspectRatio(DEGTORAD*new_fov, aspect_ratio);
	}
}

void Camera::SetFarPlane(const float & new_fp)
{
	frustum.SetViewPlaneDistances(frustum.nearPlaneDistance, new_fp);
}

void Camera::SetNearPlane(const float & new_np)
{
	frustum.SetViewPlaneDistances(new_np, frustum.farPlaneDistance);
}

void Camera::SetAspectRatio(const float & new_ar)
{
	if (new_ar <= 0)
	{
		CONSOLE_LOG_WARNING("Can't set a fov of negative value : %d", new_ar);
	}
	else
	{
		frustum.SetVerticalFovAndAspectRatio(vertical_fov, new_ar);
	}
}

float Camera::GetVerticalFOV() const
{
	return frustum.VerticalFov() * RADTODEG;
}

float Camera::GetHorizontalFOV() const
{
	return frustum.HorizontalFov() * RADTODEG;
}

float Camera::GetFarPlane() const
{
	return frustum.FarPlaneDistance();
}

float Camera::GetNearPlane() const
{
	return frustum.NearPlaneDistance();
}

float Camera::GetAspectRatio() const
{
	return aspect_ratio;
}

 float * Camera::GetViewMatrix()
{
	return  &ViewMatrix[0][0];
}

 float * Camera::GetProjectionMatrix()
 {
	 return nullptr;
 }


bool Camera::IsCulling() const
{
	return is_culling == true;
}

void Camera::SetCulling(bool culling)
{
	is_culling = culling;
}

Frustum Camera::GetFrustum()const
{
	return frustum;
}

// -----------------------------------------------------------------
void Camera::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);
	ViewMatrixInverse = ViewMatrix.Inverted();
}

void Camera::UpdatePosition(float3 newpos)
{
	Position += newpos;
	Reference += newpos;
}

void Camera::Look(const float3 & Position, const float3 & Reference, bool Pivoting)
{
	this->Position = Position;
	this->Reference = Reference;


	float3 diff = Position - Reference;
	Z = diff.Normalized();
	float3 YcrossZ = float3(0.0f, 1.0f, 0.0f).Cross(Z);
	X = YcrossZ.Normalized();
	Y = Z.Cross(X);

	if (!Pivoting)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void Camera::LookAt(const float3 & at)
{
	Reference = at;


	float3 diff = Position - Reference;
	Z = diff.Normalized();
	float3 YcrossZ = float3(0.0f, 1.0f, 0.0f).Cross(Z);
	X = YcrossZ.Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}

void Camera::HandleMouse()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	Position -= Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * App->camera->GetMouseSensitivity();

		X = Rotate(X, DeltaX, float3(0.0f, 1.0f, 0.0f));
		Y = Rotate(Y, DeltaX, float3(0.0f, 1.0f, 0.0f));
		Z = Rotate(Z, DeltaX, float3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy *  App->camera->GetMouseSensitivity();

		Y = Rotate(Y, DeltaY, X);
		Z =Rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = Z.Cross(X);
		}
	}

	Position = Reference + Z * Position.Length();
}

float3 Camera::Rotate(const float3 & u, float angle, const float3 & v)
{
	return *(float3*)&(float4x4::RotateAxisAngle(v, angle) * float4(u, 1.0f));
}

void Camera::CreateNewFrustum()
{
	frustum.GetCornerPoints(frustum_vertices);
}

void Camera::DrawFrustum()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_LINES);

	glVertex3fv((GLfloat*)&frustum_vertices[1]);
	glVertex3fv((GLfloat*)&frustum_vertices[5]);
	glVertex3fv((GLfloat*)&frustum_vertices[7]);
	glVertex3fv((GLfloat*)&frustum_vertices[3]);

	glVertex3fv((GLfloat*)&frustum_vertices[3]);
	glVertex3fv((GLfloat*)&frustum_vertices[1]);

	glVertex3fv((GLfloat*)&frustum_vertices[4]);
	glVertex3fv((GLfloat*)&frustum_vertices[0]);
	glVertex3fv((GLfloat*)&frustum_vertices[2]);
	glVertex3fv((GLfloat*)&frustum_vertices[6]);

	glVertex3fv((GLfloat*)&frustum_vertices[6]);
	glVertex3fv((GLfloat*)&frustum_vertices[4]);

	glVertex3fv((GLfloat*)&frustum_vertices[5]);
	glVertex3fv((GLfloat*)&frustum_vertices[4]);
	glVertex3fv((GLfloat*)&frustum_vertices[6]);
	glVertex3fv((GLfloat*)&frustum_vertices[7]);

	glVertex3fv((GLfloat*)&frustum_vertices[7]);
	glVertex3fv((GLfloat*)&frustum_vertices[5]);

	glVertex3fv((GLfloat*)&frustum_vertices[0]);
	glVertex3fv((GLfloat*)&frustum_vertices[1]);
	glVertex3fv((GLfloat*)&frustum_vertices[3]);
	glVertex3fv((GLfloat*)&frustum_vertices[2]);

	glVertex3fv((GLfloat*)&frustum_vertices[2]);
	glVertex3fv((GLfloat*)&frustum_vertices[6]);

	glVertex3fv((GLfloat*)&frustum_vertices[3]);
	glVertex3fv((GLfloat*)&frustum_vertices[7]);
	glVertex3fv((GLfloat*)&frustum_vertices[6]);
	glVertex3fv((GLfloat*)&frustum_vertices[2]);

	glVertex3fv((GLfloat*)&frustum_vertices[2]);
	glVertex3fv((GLfloat*)&frustum_vertices[0]);

	glVertex3fv((GLfloat*)&frustum_vertices[0]);
	glVertex3fv((GLfloat*)&frustum_vertices[4]);
	glVertex3fv((GLfloat*)&frustum_vertices[5]);
	glVertex3fv((GLfloat*)&frustum_vertices[1]);

	glEnd();

}
