#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "TextureMSAA.h"
#include "Camera.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled)
{
	editor_camera = new Camera();
	cams_list.push_back(editor_camera);

	GetCurrentCam()->CalculateViewMatrix();

	GetCurrentCam()->X = float3(1.0f, 0.0f, 0.0f);
	GetCurrentCam()->Y = float3(0.0f, 1.0f, 0.0f);
	GetCurrentCam()->Z = float3(0.0f, 0.0f, 1.0f);

	GetCurrentCam()->Position = float3(3.0f, 8.0f, -8.0f);
	GetCurrentCam()->Reference = float3(0.0f, 0.0f, 0.0f);

}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;
	viewport_texture = new TextureMSAA();
	viewport_texture->Create(App->window->screen_surface->w, App->window->screen_surface->h,2);
	App->profiler.SaveInitData("Camera");
	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

void ModuleCamera3D::DrawModuleConfig()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[0]);
		if (ImGui::SliderFloat3("Position", (float*)&GetCurrentCam()->Position, -100.0f, 100.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[1]);
			App->audio->camera_tick_arr[1] = FALSEBOOL;
		}
		ImGui::Spacing();
		if (ImGui::Button("Reset"))
		{
			GetCurrentCam()->Position.Set(0, 5, 10);
			LookAt({ 0, 0, 0 });
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[2]);
		}
		else
			App->audio->camera_tick_arr[2] = FALSEBOOL;
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		if (ImGui::DragFloat("Speed", &speed_base, 1, 0.0f, 10.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[3]);
			App->audio->camera_tick_arr[3] = FALSEBOOL;
		}				
		ImGui::Spacing();
		if (ImGui::DragFloat("Wheel Speed", &wheel_speed_base, 1, 0.0f, 10.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[4]);
			App->audio->camera_tick_arr[4] = FALSEBOOL;
		}			
		ImGui::Spacing();
		if (ImGui::DragFloat("Rotation Speed", &mouse_sensitivity, 0.1, 0.0f, 2.0f))
		{
			App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[5]);
			App->audio->camera_tick_arr[5] = FALSEBOOL;
		}
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

	//Profiler
	App->profiler.SaveRunTimeData("Camera");
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	editor_camera->Look(Position, Reference, RotateAroundReference);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	editor_camera->LookAt(Spot);
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &speed)
{
	float3 newPos(0, 0, 0);

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= editor_camera->Z.Mul(speed);
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += editor_camera->Z.Mul(speed);

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= editor_camera->X.Mul(speed);
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += editor_camera->X.Mul(speed);

	editor_camera->UpdatePosition(newPos);
	
}
void ModuleCamera3D::MoveCam(const float3 &speed)
{
	float3 newPos(speed.x, speed.y, speed.z);


	editor_camera->SetPosition(newPos);
	editor_camera->SetReference(newPos);
}

bool ModuleCamera3D::MouseOverScene(PanelScene* Scene)
{
	bool ret = false;
	
	return ret;
}

Camera * ModuleCamera3D::GetCurrentCam() const
{
	return editor_camera;
}

void ModuleCamera3D::WheelMove(const float3 & mouse_speed, int direction)
{
	float3 newPos(0, 0, 0);

	if (direction == 1)
		newPos -= editor_camera->Z.Mul(mouse_speed);
	else
		newPos += editor_camera->Z.Mul(mouse_speed);

	editor_camera->UpdatePosition(newPos);

}

void ModuleCamera3D::HandleMouse()
{
	editor_camera->HandleMouse();
}

void ModuleCamera3D::Orbit()
{
	HandleMouse();
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
		HandleMouse();
		Move({ speed ,speed ,speed });
	}
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
		Orbit();
	//MOUSE WHEEL
	int wheel = App->input->GetMouseZ();
	float wheel_speed = wheel_speed_base * dt * 100;
	if (App->input->GetMouseZ() != 0)
		WheelMove({ wheel_speed ,wheel_speed ,wheel_speed }, wheel);
	//Look at Target
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		LookAt({ 0,0,0 });
	// Recalculate matrix -------------
	GetCurrentCam()->CalculateViewMatrix();
}

float ModuleCamera3D::GetSpeed() const
{
	return speed_base;
}

TextureMSAA * ModuleCamera3D::SceneMSAA()
{
	return viewport_texture;
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

void ModuleCamera3D::AdaptCamera(AABB bounding_box)
{
	//TODO PAU
	//float z = bounding_box.Diagonal().Length() * 0.4;
	//float x = bounding_box.Diagonal().Length() * 0.1;
	//float y = bounding_box.Diagonal().Length() * 0.5;
	float3 newpos = bounding_box.CenterPoint();
	newpos.z -= bounding_box.Diagonal().Length();
	newpos.y += bounding_box.Diagonal().Length();
	MoveCam({newpos.x,newpos.y,-newpos.z});
	LookAt({ 0,0,0 });
	editor_camera->CalculateViewMatrix();
}



