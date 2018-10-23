#include "Camera.h"
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include "ModuleCamera3D.h"
#include "Application.h"
#include "TextureMSAA.h"

Camera::Camera()
{

	frustum.SetPos(float3::zero);
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);
	SetFOV(60);
	frustum.nearPlaneDistance = 0.3;//needs to be higher than 0.4
	frustum.farPlaneDistance = 1000;

	frustum.type = FrustumType::PerspectiveFrustum;

	//frustum.SetWorldMatrix(float3x4::identity);



	CreateNewFrustum();
	
}

Camera::~Camera()
{
}

void Camera::SetPosition(const float3 & new_pos)
{
	frustum.SetPos(new_pos);
	//Position = frustum.pos;
	//CalculateViewMatrix();
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
		frustum.verticalFov = new_fov * DEGTORAD;
		frustum.horizontalFov = math::Atan(aspect_ratio * math::Tan(frustum.verticalFov / 2)) * 2;
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
		aspect_ratio = new_ar;
		frustum.horizontalFov = math::Atan(aspect_ratio * math::Tan(frustum.verticalFov / 2)) * 2;
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



 float * Camera::GetViewMatrix()const
{
	 static float4x4 matrix;
	 matrix = frustum.ViewMatrix();
	 matrix.Transpose();

	 return (float*)matrix.v;
}

 float * Camera::GetProjMatrix() const
 {
	 static float4x4 matrix;
	 matrix = frustum.ProjectionMatrix();
	 matrix.Transpose();

	 return (float*)matrix.v;
 }

 void Camera::UpdateProjectionMatrix()
 {
	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();

	 glLoadMatrixf(GetProjMatrix());

	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
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
//void Camera::CalculateViewMatrix()
//{
//	frustumViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);
//	ViewMatrixInverse = ViewMatrix.Inverted();
//}

void Camera::UpdatePosition(float3 newpos)
{
	//TODO
	//Position += newpos;

	//CalculateViewMatrix();
}

void Camera::Look(const float3 & Position, const float3 & Reference, bool Pivoting)
{

}

void Camera::LookAt(const float3 & at)
{
	Frustum* editor_frustum = &App->camera->editor_camera->GetCamera()->GetFrustum();
	float3 direction = at - editor_frustum->pos;

	float3x3 matrix = float3x3::LookAt(editor_frustum->front, direction.Normalized(), editor_frustum->up, float3::unitY);

	editor_frustum->front = matrix.MulDir(editor_frustum->front).Normalized();
	editor_frustum->up = matrix.MulDir(editor_frustum->up).Normalized();
}
TextureMSAA * Camera::SceneMSAA()
{
	return viewport_texture;
}
void Camera::HandleMouse()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	Frustum* editor_frustum = &App->camera->editor_camera->GetCamera()->frustum;
	if (dx != 0)
	{
		Quat X_rot = Quat::RotateY(dx);
		editor_frustum->SetFront(X_rot.Mul(editor_frustum->front).Normalized());
		editor_frustum->SetUp(X_rot.Mul(editor_frustum->up).Normalized());
	}

	if (dy != 0)
	{
		Quat rotation_y = Quat::RotateAxisAngle(editor_frustum->WorldRight(), dy);

		float3 new_up = rotation_y.Mul(editor_frustum->up).Normalized();

		if (new_up.y > 0.0f)
		{
			editor_frustum->up = new_up;
			editor_frustum->front = rotation_y.Mul(editor_frustum->front).Normalized();
		}
	}

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
