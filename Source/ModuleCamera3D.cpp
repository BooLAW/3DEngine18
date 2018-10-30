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
	//CONSOLE_LOG("Setting up the camera");
	bool ret = true;
	//editor_camera = new GameObject();
	//editor_camera->SetName("Editor Camera");
	////editor_camera->SetParent(App->scene_intro->scene_root);
	//float3 root_pos = float3::zero;
	//Quat root_rotation = Quat::identity;
	//float3 root_scale = float3::one;
	//editor_camera->transform->SetTransform(root_pos, root_rotation, root_scale);
	//editor_camera->transform->SetGlobalPos({ 0, 10, 0 });
	//editor_camera->transform->SetLocalPos({ 0,0,0 });
	//editor_camera->transform->UpdateTransformValues();

	

	//App->scene_intro->go_list.push_back(editor_camera);

	App->profiler.SaveInitData("Camera");
	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	//LOG("Cleaning camera");

	return true;
}

void ModuleCamera3D::DrawModuleConfig()
{
	//MODULE CAMERA ONLY DRAWS EDITOR CAMERA INFO, NOT CURRENT CAMERA INFO
	if (ImGui::CollapsingHeader("Camera"))
	{
		Camera* aux_cam = App->camera->editor_cam;
		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[0]);
		float3 aux_pos = aux_cam->frustum	.pos;
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
		if (ImGui::SliderFloat("Far Plane", &fp, 50.0f, 1000.f))
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
					App->camera->AdaptCamera(App->scene_intro->GetSelected()->GetBB(), App->scene_intro->GetSelected()->transform->transform->pos);
				else
					App->camera->AdaptCamera(App->scene_intro->GetSelected()->transform->transform->pos);
			}
			else
				CONSOLE_LOG_INFO("Select GameObject in the hierarchy to focus");
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[4]);

			aux_cam->SetFOV(80);
			aux_cam->SetNearPlane(0.5);
			aux_cam->SetFarPlane(1000);

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
		if (ImGui::Checkbox("Draw Frustum", &draw_frustum))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[7]);
		}
		else
			App->audio->camera_tick_arr[7] = FALSEBOOL;


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
	if (draw_frustum) {
		editor_cam->DrawFrustum();
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
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_UP) {}
	if(!newPos.IsZero())
		editor_cam->frustum.Translate(newPos);

	
}
void ModuleCamera3D::MoveCam(const float3 &pos)
{

	editor_cam->frustum.Translate(pos);

	
}

bool ModuleCamera3D::MouseOverScene(PanelScene* Scene)
{
	bool ret = false;
	
	return ret;
}

Camera * ModuleCamera3D::GetCurrentCam() const
{	
	if (editor_cam != nullptr)
		return editor_cam;
	else
		return nullptr;
}

void ModuleCamera3D::StartEditorCamera()
{
	//editor_camera->SetParent(App->scene_intro->scene_root);
	//ComponentCamera* c_camera = new ComponentCamera();
	//c_camera->SetOwner(editor_camera);
	//c_camera->Start();
	//editor_camera->PushComponent(c_camera);
	editor_cam = new Camera();
	editor_cam->viewport_texture = new TextureMSAA();
	editor_cam->viewport_texture->Create(1500, 1000, 2);
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
	//aux->UdatePos();

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
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		HandleMouse(dt);
		Move(speed);
	}
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
		Orbit(dt);
	//MOUSE WHEEL
	int wheel = App->input->GetMouseZ();
	float wheel_speed = wheel_speed_base * dt * 100;
	if (App->input->GetMouseZ() != 0)
		WheelMove(wheel_speed, wheel);
	//Look at Target
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (App->scene_intro->GetSelected() != nullptr)
		{
			if (App->scene_intro->GetSelected()->HasMesh())
				AdaptCamera(App->scene_intro->GetSelected()->GetBB(), App->scene_intro->GetSelected()->transform->transform->pos);
			else
				AdaptCamera(App->scene_intro->GetSelected()->transform->transform->pos);
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

	float3 newpos = transformpos;
	newpos.y += 10;
	newpos.z += 10;

	editor_cam->frustum.pos.Set(newpos.x, newpos.y, newpos.z);

	LookAt(transformpos);
}



