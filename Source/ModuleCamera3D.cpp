#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "TextureMSAA.h"
#include "Camera.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"


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
	LOG("Setting up the camera");
	bool ret = true;
	editor_camera = new GameObject();
	editor_camera->SetName("Main Camera");
	//editor_camera->SetParent(App->scene_intro->scene_root);
	float3 root_pos = float3::zero;
	Quat root_rotation = Quat::identity;
	float3 root_scale = float3::one;
	editor_camera->transform->SetTransform(root_pos, root_rotation, root_scale);
	editor_camera->transform->SetGlobalPos({ 0, 0, 0 });
	editor_camera->transform->SetLocalPos({ 0,0,0 });
	editor_camera->transform->UpdateTransformValues();

	

	App->scene_intro->go_list.push_back(editor_camera);

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
	//	if (ImGui::SliderFloat3("Position", (float*)&GetCurrentCam()->Position, -100.0f, 100.0f))
	//	{
	//		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[1]);
	//		App->audio->camera_tick_arr[1] = FALSEBOOL;
	//	}
	//	ImGui::Spacing();
	//	if (ImGui::Button("Reset"))
	//	{
	//		//GetCurrentCam()->Position.Set(0, 5, 10);
	//		LookAt({ 0, 0, 0 });
	//		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[2]);
	//	}
	//	else
	//		App->audio->camera_tick_arr[2] = FALSEBOOL;
	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();
	//	if (ImGui::DragFloat("Speed", &speed_base, 1, 0.0f, 10.0f))
	//	{
	//		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[3]);
	//		App->audio->camera_tick_arr[3] = FALSEBOOL;
	//	}				
	//	ImGui::Spacing();
	//	if (ImGui::DragFloat("Wheel Speed", &wheel_speed_base, 1, 0.0f, 10.0f))
	//	{
	//		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[4]);
	//		App->audio->camera_tick_arr[4] = FALSEBOOL;
	//	}			
	//	ImGui::Spacing();
	//	if (ImGui::DragFloat("Rotation Speed", &mouse_sensitivity, 0.1, 0.0f, 2.0f))
	//	{
	//		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[5]);
	//	}
	//	else
	//		App->audio->camera_tick_arr[5] = FALSEBOOL;


	//	ImGui::Spacing();
	//	if (ImGui::Checkbox("Draw Frustum", &draw_frustum))
	//	{
	//		App->audio->PlayFx(LIGHT_BUTTON_CLICK, &App->audio->camera_tick_arr[6]);
	//	}
	//	else
	//		App->audio->camera_tick_arr[6] = FALSEBOOL;


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
		editor_camera->GetCamera()->DrawFrustum();
	}
	//Profiler
	App->profiler.SaveRunTimeData("Camera");
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	ComponentCamera* aux = (ComponentCamera*)editor_camera->GetComponent(CAMERA);
	aux->cam->Look(Position, Reference, RotateAroundReference);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	ComponentCamera* aux = (ComponentCamera*)editor_camera->GetComponent(CAMERA);

	aux->cam->LookAt(Spot);
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &speed)
{
	ComponentCamera* aux = (ComponentCamera*)editor_camera->GetComponent(CAMERA);

	float3 newPos(0, 0, 0);

	//if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= aux->cam->Z.Mul(speed);
//	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += aux->cam->Z.Mul(speed);

	//if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= aux->cam->X.Mul(speed);
	//if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += aux->cam->X.Mul(speed);

	aux->cam->UpdatePosition(newPos);
	
}
void ModuleCamera3D::MoveCam(const float3 &speed)
{
	ComponentCamera* aux = (ComponentCamera*)editor_camera->GetComponent(CAMERA);

	float3 newPos(speed.x, speed.y, speed.z);


	//aux->cam->SetPosition(newPos);
//	aux->cam->SetReference(newPos);
	
}

bool ModuleCamera3D::MouseOverScene(PanelScene* Scene)
{
	bool ret = false;
	
	return ret;
}

Camera * ModuleCamera3D::GetCurrentCam() const
{
	ComponentCamera* aux;
	if (editor_camera != nullptr)
	 aux = (ComponentCamera*)editor_camera->GetComponent(CAMERA);
	else
		return nullptr;
	
	if (aux != nullptr)
		return aux->cam;
	else
		return nullptr;
}

void ModuleCamera3D::StartEditorCamera()
{
	editor_camera->SetParent(App->scene_intro->scene_root);
	ComponentCamera* c_camera = new ComponentCamera();
	c_camera->SetOwner(editor_camera);
	c_camera->Start();
	editor_camera->PushComponent(c_camera);

	c_camera->cam->viewport_texture = new TextureMSAA();
	c_camera->cam->viewport_texture->Create(1500, 1000, 2);
}

void ModuleCamera3D::WheelMove(const float3 & mouse_speed, int direction)
{
	ComponentCamera* aux = (ComponentCamera*)editor_camera->GetComponent(CAMERA);

	float3 newPos(0, 0, 0);

	/*if (direction == 1)
		newPos -= aux->cam->Z.Mul(mouse_speed);
	else
		newPos += aux->cam->Z.Mul(mouse_speed);*/

	aux->cam->UpdatePosition(newPos);

}

void ModuleCamera3D::HandleMouse()
{
	ComponentCamera* aux = (ComponentCamera*)editor_camera->GetComponent(CAMERA);

	aux->cam->HandleMouse();
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
	//GetCurrentCam()->CalculateViewMatrix();
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

void ModuleCamera3D::AdaptCamera(AABB bounding_box)
{
	ComponentCamera* aux = (ComponentCamera*)editor_camera->GetComponent(CAMERA);

	float3 newpos = bounding_box.CenterPoint();
	newpos.z -= bounding_box.Diagonal().Length();
	newpos.y += bounding_box.Diagonal().Length();
	MoveCam({newpos.x,newpos.y,-newpos.z});
	LookAt({ 0,0,0 });
	//aux->cam->CalculateViewMatrix();
}



