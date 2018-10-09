#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "TextureMSAA.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled)
{

	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(3.0f, 8.0f, -8.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;
	viewport_texture = new TextureMSAA();
	viewport_texture->Create(App->window->screen_surface->w, App->window->screen_surface->h);
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
		ImGui::DragFloat("Speed", &speed_base, 1, 0.0f, 10.0f);
		ImGui::DragFloat("Wheel Speed", &wheel_speed_base, 1, 0.0f, 10.0f);
		ImGui::DragFloat("Rotation Speed", &mouse_sensitivity, 0.1, 0.0f, 2.0f);

	}
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
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &speed)
{
	vec3 newPos(0, 0, 0);

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	Position += newPos;
	Reference += newPos;
}
void ModuleCamera3D::MoveCam(const vec3 &speed)
{
	vec3 newPos(speed.x, speed.y, speed.z);



	Position = newPos;
	Reference = newPos;
}

void ModuleCamera3D::WheelMove(const vec3 & mouse_speed, int direction)
{
	vec3 newPos(0, 0, 0);

	if (direction == 1)
		newPos -= Z * mouse_speed;
	else
		newPos += Z * mouse_speed;

	Position += newPos;
	Reference += newPos;
}

void ModuleCamera3D::HandleMouse()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	Position -= Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * mouse_sensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * mouse_sensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}

	Position = Reference + Z * length(Position);
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
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
	Move(speed);
	// MOUSE MOTION ----------------
	// ROTATION
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		HandleMouse();
	//MOUSE WHEEL
	int wheel = App->input->GetMouseZ();
	float wheel_speed = wheel_speed_base * dt * 100;
	if (App->input->GetMouseZ() != 0)
		WheelMove(wheel_speed, wheel);
	//Look at Target
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		LookAt({ 0,0,0 });
	// Recalculate matrix -------------
	CalculateViewMatrix();
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
	MoveCam({newpos.x,newpos.y,newpos.z});
	LookAt({ 0,0,0 });
	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

