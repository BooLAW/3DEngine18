#include "ComponentCamera.h"
#include "TextureMSAA.h"
#include "Camera.h"
#include "Application.h"
#include "ModuleWindow.h"

ComponentCamera::ComponentCamera()
{
	type = CAMERA;
	SetName("Component Camera");

	cam = new Camera();
}

bool ComponentCamera::Start()
{
	viewport_texture = new TextureMSAA();
	viewport_texture->Create(App->window->screen_surface->w, App->window->screen_surface->h, 2);
	return true;
}

bool ComponentCamera::Update()
{
	//debug draw for the frustum
	return false;
}


ComponentCamera::~ComponentCamera()
{
}

Camera * ComponentCamera::GetCamera()
{
	return cam;
}

TextureMSAA * ComponentCamera::SceneMSAA()
{
	return viewport_texture;
}
