#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "TextureMSAA.h"
#include "Camera.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ModuleScene.h"
#include "PanelScene.h"
#include "DebugDraw.h"
#include "ImGui/ImGuizmo.h"


ModuleCamera3D::ModuleCamera3D(bool start_enabled)
{
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init()
{
	
	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	CONSOLE_LOG_INFO("Setting up the camera");
	bool ret = true;
	
	App->profiler.SaveInitData("Camera");
	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	CONSOLE_LOG_INFO("Cleaning camera");

	return true;
}

void ModuleCamera3D::DrawModuleConfig()
{
	//MODULE CAMERA ONLY DRAWS EDITOR CAMERA INFO, NOT CURRENT CAMERA INFO
	if (ImGui::CollapsingHeader("Camera"))
	{
		Camera* aux_cam = App->camera->editor_cam;
		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[0]);
		float3 aux_pos = aux_cam->frustum.pos;
		if (ImGui::SliderFloat3("Position", (float*)&aux_pos, -100.0f, 100.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[1]);
			aux_cam->frustum.pos = aux_pos;
			App->audio->camera_tick_arr[1] = FALSEBOOL;
		}
		ImGui::Spacing();
		float  a = aux_cam->GetAspectRatio();
		if (ImGui::SliderFloat("Aspect Ratio", &a, 0.0f, 2.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[2]);
			aux_cam->SetAspectRatio(a);
			App->audio->camera_tick_arr[2] = FALSEBOOL;
		}

		float  f = aux_cam->GetVerticalFOV();
		if (ImGui::SliderFloat("FOV", &f, 45.0f, 90.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[3]);
			aux_cam->SetFOV(f);
			App->audio->camera_tick_arr[3] = FALSEBOOL;
		}
		ImGui::Spacing();

		float  np = aux_cam->GetNearPlane();
		if (ImGui::SliderFloat("Near Plane", &np, 0.5, 10.0))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[8]);
			aux_cam->SetNearPlane(np);
			App->audio->camera_tick_arr[8] = FALSEBOOL;
		}
		ImGui::Spacing();

		float  fp = aux_cam->GetFarPlane();
		if (ImGui::SliderFloat("Far Plane", &fp, 50.0f, 500.f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[9]);
			aux_cam->SetFarPlane(fp);
			App->audio->camera_tick_arr[9] = FALSEBOOL;
		}
		ImGui::Spacing();

		if (ImGui::Button("Reset"))
		{
			if (App->scene_intro->GetSelected() != nullptr)
			{
				if (App->scene_intro->GetSelected()->HasMesh())
					App->camera->AdaptCamera(App->scene_intro->GetSelected()->GetBB(), App->scene_intro->GetSelected()->comp_transform->transform.pos);
				else
					App->camera->AdaptCamera(App->scene_intro->GetSelected()->comp_transform->transform.pos);
			}
			else
				CONSOLE_LOG_INFO("Select GameObject in the hierarchy to focus");
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[4]);

			aux_cam->SetFOV(80);
			aux_cam->SetNearPlane(0.5);
			aux_cam->SetFarPlane(500);

		}
		else
			App->audio->camera_tick_arr[4] = FALSEBOOL;
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		if (ImGui::DragFloat("Speed", &speed_base, 1, 0.0f, 10.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[4]);
			App->audio->camera_tick_arr[4] = FALSEBOOL;
		}				
		ImGui::Spacing();
		if (ImGui::DragFloat("Wheel Speed", &wheel_speed_base, 1, 0.0f, 10.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[5]);
			App->audio->camera_tick_arr[5] = FALSEBOOL;
		}			
		ImGui::Spacing();
		if (ImGui::DragFloat("Rotation Speed", &mouse_sensitivity, 0.1, 0.0f, 2.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[6]);
		}
		else
			App->audio->camera_tick_arr[6] = FALSEBOOL;

		ImGui::Spacing();
		if (ImGui::Checkbox("Draw Ray", &draw_mouse_ray))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[8]);
		}
		else
			App->audio->camera_tick_arr[8] = FALSEBOOL;


	}
	else
		App->audio->camera_tick_arr[0] = FALSEBOOL;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	//Profiler
	App->profiler.StartTimer("Camera");
	if (!locked)
		CameraMovement(dt);	
	//Mouse Picking
	bool mouse_picking_working = true;
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && CheckMouseInWindow(App->input->GetMouseX(), App->input->GetMouseY()) && mouse_picking_working && !ImGuizmo::IsOver())
	{
		CreateRayTest(App->input->GetMouseX(), App->input->GetMouseY());
	}
	
	//Profiler
	App->profiler.SaveRunTimeData("Camera");
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	editor_cam->Look(Position, Reference, RotateAroundReference);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	editor_cam->LookAt(Spot);
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float &speed)
{
	float3 newPos(0, 0, 0);

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += editor_cam->frustum.front * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= editor_cam->frustum.front * speed;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= editor_cam->frustum.WorldRight() * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += editor_cam->frustum.WorldRight() * speed;


	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -=  speed;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y +=  speed;
	if(!newPos.IsZero())
		editor_cam->frustum.Translate(newPos);

	
}
void ModuleCamera3D::MoveCam(const float3 &pos)
{

	editor_cam->frustum.Translate(pos);

	
}

void ModuleCamera3D::SetCurrentCam(GameObject * cam)
{
	if(cam!= nullptr)
		current_game_camera = cam;
}

bool ModuleCamera3D::MouseOverScene(PanelScene* Scene)
{
	bool ret = false;
	
	return ret;
}

Camera * ModuleCamera3D::GetCurrentCam() const
{	
	Camera* ret = nullptr;
	if (current_game_camera == nullptr)
	{
		CONSOLE_LOG_WARNING("No current Camera for Game");
	}
	else
	{
		if (current_game_camera->HasCam())
			ret = current_game_camera->GetCamera();
	}
	return ret;
}

Camera * ModuleCamera3D::GetEditorCam() const
{
	if (editor_cam != nullptr)
		return editor_cam;
	else
		return nullptr;
}

void ModuleCamera3D::StartEditorCamera()
{
	editor_cam = new Camera();
	editor_cam->viewport_texture = new TextureMSAA();
	editor_cam->viewport_texture->Create(1500, 1000, 2);
}
void ModuleCamera3D::StartNewCamera()
{
	Camera* aux = current_game_camera->GetCamera();
	aux = new Camera();
	current_game_camera->GetCamera()->viewport_texture = new TextureMSAA();
	current_game_camera->GetCamera()->viewport_texture->Create(1500, 1000, 2);
}
void ModuleCamera3D::CreateRayTest(int x, int y)
{
	//Serialize  x & y
	float n_x = (((x - App->imgui->scene->GetPos().x) / App->imgui->scene->GetSize().x) ) ;
	float n_y = (((y - App->imgui->scene->GetPos().y) / App->imgui->scene->GetSize().y));
	
	n_x -= 0.5;
	n_x *= 2;

	n_y -= 0.5;
	n_y *= -2;

	//Create Ray or LineSegment??
	LineSegment picking;
	if (n_x > -1 && n_x < 1 && n_y > -1 && n_y < 1)
	{
		picking = GetEditorCam()->frustum.UnProjectLineSegment(n_x, n_y);
		debug_mouse_ray = picking;
	}
	//Check Collisions
	if(picking.Length()!=0)
		App->scene_intro->ClickSelection(picking);
}

bool ModuleCamera3D::CheckMouseInWindow(int x, int y)
{
	ImVec2 pos_w = App->imgui->scene->GetPos();
	ImVec2 size_w = App->imgui->scene->GetSize();
	return (x > pos_w.x && y > pos_w.y && x < pos_w.x + size_w.x && y < pos_w.y + size_w.y);

}

void ModuleCamera3D::DrawRay()
{
	glBegin(GL_LINES);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(10.0f);
	glDisable(GL_CULL_FACE);
	glColor3f(255, 0, 255);

	glVertex3fv((GLfloat*)&debug_mouse_ray.a);
	glVertex3fv((GLfloat*)&debug_mouse_ray.b);

	glEnd();
	glLineWidth(1.0f);
	glColor3f(255, 255, 255);
	glEnable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModuleCamera3D::WheelMove(const float & mouse_speed, int direction)
{
	float3 newPos(0, 0, 0);

	if (direction == 1)
		newPos -= editor_cam->frustum.front * mouse_speed;
	else
		newPos += editor_cam->frustum.front * mouse_speed;
	
	if(!newPos.IsZero())
		editor_cam->frustum.Translate(newPos);

}

void ModuleCamera3D::HandleMouse(const float dt)
{
	editor_cam->HandleMouse(dt);
}

void ModuleCamera3D::Orbit(const float dt)
{
	HandleMouse(dt);
	LookAt({ 0,0,0 });
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	//make a current camera once we have game mode
	return GetCurrentCam()->GetViewMatrix();
}

void ModuleCamera3D::SetSpeed(float new_speed)
{
	speed_base = new_speed; 
}

void ModuleCamera3D::SetMouseSensitivity(float new_sensitivity)
{
	mouse_sensitivity = new_sensitivity; 
}

float ModuleCamera3D::GetMouseSensitivity() const
{
	return mouse_sensitivity;
}

void ModuleCamera3D::CameraMovement(float dt)
{
	//Define Speed	
	float speed = speed_base * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 2.0f * dt * speed_base;
	//WASD MOVEMENT

	// MOUSE MOTION ----------------
	// ROTATION
	if (App->imgui->scene->MouseOver() && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		HandleMouse(dt);
		Move(speed);
	}
	if (App->imgui->scene->MouseOver() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
		Orbit(dt);
	//MOUSE WHEEL
	int wheel = App->input->GetMouseZ();
	float wheel_speed = wheel_speed_base * dt * 100;
	if (App->imgui->scene->MouseOver() && App->input->GetMouseZ() != 0)
		WheelMove(wheel_speed, wheel);
	//Look at Target
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (App->scene_intro->GetSelected() != nullptr)
		{
			if (App->scene_intro->GetSelected()->HasMesh())
				AdaptCamera(App->scene_intro->GetSelected()->GetBB(), App->scene_intro->GetSelected()->comp_transform->transform.pos);
			else
				AdaptCamera(App->scene_intro->GetSelected()->comp_transform->transform.pos);
		}
		else
			CONSOLE_LOG_INFO("Select GameObject in the hierarchy to focus");
	}
	
	

}

float ModuleCamera3D::GetSpeed() const
{
	return speed_base;
}


void ModuleCamera3D::LockCamera()
{
	locked = true; 
}

void ModuleCamera3D::UnlockCamera()
{
	locked = false; 
}

bool ModuleCamera3D::IsLocked()
{
	return locked;
}

bool ModuleCamera3D::Save(Document & config_w, FileWriteStream & os)
{
	Document::AllocatorType& allocator = config_w.GetAllocator();

	Value test(kObjectType);

	test.AddMember("SpeedBase", speed_base, allocator);
	test.AddMember("MouseSensitivity", mouse_sensitivity, allocator);
	test.AddMember("MouseWheelSpeed", wheel_speed_base, allocator);

	config_w.AddMember("camera", test, allocator);

	return true;
}

bool ModuleCamera3D::Load(Document * config_r)
{
	Document ret;

	ret.Parse(App->loadBuf);
	ret.IsObject();
	speed_base = ret["camera"]["SpeedBase"].GetFloat();
	mouse_sensitivity = ret["camera"]["MouseSensitivity"].GetFloat();
	wheel_speed_base = ret["camera"]["MouseWheelSpeed"].GetFloat();

	return true;
}

void ModuleCamera3D::AdaptCamera(AABB bounding_box,float3 transformpos)
{
	
	float3 newpos = bounding_box.CenterPoint();
	newpos.z -= (bounding_box.Diagonal().Length());
	newpos.y += (bounding_box.Diagonal().Length());
	newpos.x += transformpos.x;
	newpos.y += transformpos.y;
	newpos.z += transformpos.z;

	editor_cam->frustum.pos.Set(newpos.x,newpos.y,newpos.z);

	float3 look_at_pos{0,0,0};

	look_at_pos.x = bounding_box.CenterPoint().x + transformpos.x;
	look_at_pos.y = bounding_box.CenterPoint().y + transformpos.y;
	look_at_pos.z = bounding_box.CenterPoint().z + transformpos.z;

	LookAt(look_at_pos);
}
void ModuleCamera3D::AdaptCamera(float3 transformpos)
{
	editor_cam->frustum.pos.Set(transformpos.x, transformpos.y, transformpos.z);

	LookAt(transformpos);
}



