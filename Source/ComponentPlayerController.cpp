#include "ComponentPlayerController.h"
#include "ModuleSceneImGui.h"



ComponentPlayerController::ComponentPlayerController()
{
	type = CONTROLLER;
	speed = 10;
	sensitivity = 0.5;
	bullet_radius = 3;
	shoot_force = 30;
}
ComponentPlayerController::ComponentPlayerController(GameObject* owner)
{
	type = CONTROLLER;
	SetOwner(owner);
	speed = 10;
	sensitivity = 0.5;
	bullet_radius = 3;
	shoot_force = 30;
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
		CONSOLE_LOG_WARNING("Can'tset a negativy speed to player controlelr");
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
