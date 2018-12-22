#include "ComponentPlayerController.h"
#include "ModuleSceneImGui.h"



ComponentPlayerController::ComponentPlayerController()
{
	type = CONTROLLER;
	speed = 10.0f;
	sensitivity = 0.5;
	bullet_radius = 3.0f;
	shoot_force = 30.0f;
	jump_force = 10.0f;
}
ComponentPlayerController::ComponentPlayerController(GameObject* owner)
{
	type = CONTROLLER;
	SetOwner(owner);
	speed = 10.0f;
	sensitivity = 0.5f;
	bullet_radius = 3.0f;
	shoot_force = 30.0f;
	jump_force = 10.0f;
}

ComponentPlayerController::ComponentPlayerController(GameObject * owner, float* values)
{
	type = CONTROLLER;
	SetOwner(owner);
	speed = values[0];
	sensitivity = values[1];
	bullet_radius = values[2];
	shoot_force = values[3];
	jump_force = values[4];
}

ComponentPlayerController::~ComponentPlayerController()
{
}

void ComponentPlayerController::DrawInspectorInfo()
{
	ImGui::InputFloat("Speed", &speed);

	ImGui::InputFloat("Sensitivity", &sensitivity);

	ImGui::InputFloat("Bullet Radius", &bullet_radius);

	ImGui::InputFloat("Shoot Force", &shoot_force);
}

float ComponentPlayerController::GetSpeed() const
{
	return speed;
}

void ComponentPlayerController::SetSpeed(float speed)
{
	if (speed < 0)
	{
		CONSOLE_LOG_WARNING("Can't set a negativy speed to player controlelr");
		return;
	}
		this->speed = speed;
}

float ComponentPlayerController::GetShootForce() const
{
	return shoot_force;
}

void ComponentPlayerController::SetShootForce(float shoot_force)
{
	if (shoot_force < 0)
		return;
	this->shoot_force = shoot_force;
}

float ComponentPlayerController::GetBulletRadius() const
{
	return bullet_radius;
}

void ComponentPlayerController::SetBulletRadius(float radius)
{
	if (radius <= 0)
		return;
	bullet_radius = radius;
}

float ComponentPlayerController::GetPlayerSensitivity() const
{
	return sensitivity;
}

void ComponentPlayerController::SetPlayerSensitivity(float sens)
{
	if (sens <= 0)
		return;
	sensitivity = sens;
}

float ComponentPlayerController::GetJumpForce() const
{
	return jump_force;
}
