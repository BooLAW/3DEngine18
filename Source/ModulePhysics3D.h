#pragma once
#include "Module.h"
#include "Globals.h"

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D( bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	void DrawModuleConfig()override;
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void CreateSphere(vec position, int radius);
	void CreateCube(vec minPoint, vec maxPoint);
	std::list<float2> GetSphereCollisions();
	std::list<float2> GetCubeCollisions();
	
	private:

	bool debug;

	

	std::vector<AABB> cube_list;
	std::vector<Sphere> spheres_list;

};

