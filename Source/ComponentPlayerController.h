#ifndef __COMPONENT_PLAYERCONTROLLER_H__
#define __COMPONENT_PLAYERCONTROLLER_H__

#include "Component.h"
class ComponentPlayerController : public Component
{
public:
	ComponentPlayerController();
	virtual ~ComponentPlayerController();
	void DrawInspectorInfo()override;

	float GetSpeed()const;
	void SetSpeed(float speed);

	float GetShootForce() const;
	void SetShootForce(float shoot_force);
	
	float GetBulletRadius()const;
	void SetBulletRadius(float radius);

	float GetPlayerSensitivity()const;
	void SetPlayerSensitivity(float sens);
private:
	float speed;
	float shoot_force;
	float bullet_radius;
	float sensitivity;
};

#endif